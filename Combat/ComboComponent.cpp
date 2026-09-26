// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboComponent.h"
#include "CharacterBase.h"
#include "WeaponDefinition.h"
#include "Components/CapsuleComponent.h"
#include "SoftTargetComponent.h"

// Sets default values for this component's properties
UComboComponent::UComboComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComboComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Equipment = GetOwner()->FindComponentByClass<UEquipmentComponent>();
	AttributeSet = GetOwner()->FindComponentByClass<UAttributeSet>();
}


bool UComboComponent::RequestAttack(bool bHeavy)
{
	const float Now = GetWorld()->GetTimeSeconds();
	BufferedInputTime = Now;

	if (!bIsAttacking)
	{
		const bool bStarted = StartAttackAtIndex(0, bHeavy);
		BufferedInputTime = -1.0f;
		return bStarted;
	}

	if (bComboWindowOpen)
	{
		TryAdvanceCombo();
	}

	return false;
}


bool UComboComponent::TryAdvanceCombo()
{
	BufferedInputTime = -1.0f;
	return StartAttackAtIndex(CurrentChainIndex + 1, bCurrentAttackIsHeavy);
}

bool UComboComponent::StartAttackAtIndex(int32 Index, bool bHeavy)
{
	if (!Equipment || !AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 1"));
		return false;
	}

	FWeaponInstance WeaponInstance;
	if (!Equipment->GetEquippedWeaponInstance(WeaponInstance) || !WeaponInstance.Definition)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 2"));

		return false;
	}

	const TArray<FAttackData>& Chain = bHeavy ? WeaponInstance.Definition->HeavyChainData : WeaponInstance.Definition->LightChainData;

	if (!Chain.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 3"));

		return false;
	}

	const FAttackData& AttackData = Chain[Index];
	//Should be able to attack with little stamina, and will drain it all even at 1 stamina, but maybe make stamina go beneath 0 so it takes longer to attack again
	if (!(AttributeSet->GetAttributeValue(StaminaAttributeTag) > 0.0f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 4"));

		return false;
	}

	if (!AttackData.Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 5"));

		return false;
	}

	FInstantChange	InstantChangeStruct;
	InstantChangeStruct.AttributeTag = StaminaAttributeTag;
	InstantChangeStruct.Delta = AttackData.StaminaCost;
	InstantChangeStruct.BlockedByTags = AttackData.BlockedByTags;

	if (!AttributeSet->ApplyInstantChange(InstantChangeStruct))
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 6"));

		return false;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 7"));

		return false;
	}

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Fail 8"));

		return false;
	}

	if (AttackData.bUsesSoftTargeting)
	{
		ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
		if (!OwnerCharacter)
		{
			return false;
		}

		UMotionWarpingComponent* Warping = OwnerCharacter->GetMotionWarping();

		AActor* Target = nullptr;
		if (USoftTargetComponent* SoftTargeting = OwnerCharacter->GetSoftTarget())
		{
			Target = SoftTargeting->FindAttackTarget(OwnerCharacter->GetPreferredTargetDirection());
		}

		if (Target && Warping)
		{
			const FVector MyLocation = OwnerCharacter->GetActorLocation();
			const FVector TargetLocation = Target->GetActorLocation();

			//Flatten, the lunge should stay horizontal
			FVector ToTarget = TargetLocation - MyLocation;
			ToTarget.Z = 0.0f;
			const float CurrentDistance = ToTarget.Size();
			ToTarget = ToTarget.GetSafeNormal();

			//AttackReach is how far past both capsules our strike extends
			//Adding the capsule radius so different sizes work fine
			float StopDistance = AttackReach;

			if (const ACharacter* TargetCharacter = Cast<ACharacter>(Target))
			{
				StopDistance += TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
			}
			if (const ACharacter* MyCharacter = Cast<ACharacter>(OwnerCharacter))
			{
				StopDistance += MyCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
			}

			FVector WarpLocation;

			if (CurrentDistance <= StopDistance)
			{
				//Already in range, warp to our own position so the animation's Root motion is cancelled rather than amplified, Rotation still applies
				WarpLocation = MyLocation;
			}
			else
			{
				WarpLocation = TargetLocation - (ToTarget * StopDistance);
				WarpLocation.Z = MyLocation.Z;

				//Cap it so it reads as a lunge rather than a teleport
				const float WarpDistance = FVector::Dist2D(MyLocation, WarpLocation);
				if (WarpDistance > MaxWarpDistance)
				{
					WarpLocation = MyLocation + (ToTarget * MaxWarpDistance);
					WarpLocation.Z = MyLocation.Z;
				}
			}

			Warping->AddOrUpdateWarpTargetFromLocationAndRotation(
				WarpTargetName, WarpLocation, ToTarget.Rotation());
		}
		else if (Warping)
		{
			Warping->RemoveWarpTarget(WarpTargetName);
		}
	}

	AnimInstance->Montage_Play(AttackData.Montage);
	ActiveMontage = AttackData.Montage;
	FOnMontageEnded EndedDelegate;
	EndedDelegate.BindUObject(this, &UComboComponent::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndedDelegate, AttackData.Montage);

	CurrentChainIndex = Index;
	bComboWindowOpen = false;
	bIsAttacking = true;
	bCurrentAttackIsHeavy = bHeavy;

	return true;
}

void UComboComponent::OnComboWindowOpen()
{
	bComboWindowOpen = true;

	if (BufferedInputTime < 0.0f)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if ((Now - BufferedInputTime) <= InputBufferWindow)
	{
		TryAdvanceCombo();
	}
	else
	{
		BufferedInputTime = -1.0f;
	}
}

void UComboComponent::OnComboWindowClosed()
{
	bComboWindowOpen = false;
}

void UComboComponent::OnTraceWindowBegin()
{
	if (!Equipment) 
	{
		return; 
	}

	AWeaponActor* Weapon = Equipment->GetEquippedWeaponActor();
	if (!Weapon) 
	{ 
		return; 
	}

	FWeaponInstance WeaponInstance;
	if (!Equipment->GetEquippedWeaponInstance(WeaponInstance) || !WeaponInstance.Definition)
	{
		return;
	}

	const TArray<FAttackData>& Chain = bCurrentAttackIsHeavy? WeaponInstance.Definition->HeavyChainData : WeaponInstance.Definition->LightChainData;

	if (!Chain.IsValidIndex(CurrentChainIndex)) 
	{ 
		return; 
	}

	const FAttackData& AttackData = Chain[CurrentChainIndex];

	Weapon->SetTraceActive(true, AttackData.DamageMultiplier, AttackData.BlockedByTags);
}

void UComboComponent::OnTraceWindowEnd()
{
	if (Equipment)
	{
		if (AWeaponActor* Weapon = Equipment->GetEquippedWeaponActor())
		{
			Weapon->SetTraceActive(false);
		}
	}
}

bool UComboComponent::IsAttacking() const
{
	return bIsAttacking;
}

void UComboComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage)
	{
		return;
	}

	ActiveMontage = nullptr;
	ResetCombo();
}

void UComboComponent::ResetCombo()
{
	if (ActiveMontage)
	{
		UAnimMontage* MontageToStop = ActiveMontage;
		ActiveMontage = nullptr;

		if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwner()))
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(0.2f, MontageToStop);
			}
		}
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwner()))
	{
		if (UActionStateComponent* ActionState = Character->GetActionState())
		{
			if (ActionState->GetCurrentState() == ECharacterActionState::Attacking)
			{
				ActionState->ForceEnterState(ECharacterActionState::None);
			}
		}
	}



	bIsAttacking = false;
	bComboWindowOpen = false;
	CurrentChainIndex = 0;
	BufferedInputTime = -1.0f;

	if (Equipment)
	{
		if (AWeaponActor* Weapon = Equipment->GetEquippedWeaponActor())
		{
			Weapon->SetTraceActive(false);
		}
	}
}


// Called every frame
void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


