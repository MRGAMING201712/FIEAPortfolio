// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));
	ActionState = CreateDefaultSubobject<UActionStateComponent>(TEXT("ActionState"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	Combo = CreateDefaultSubobject<UComboComponent>(TEXT("Combo"));
	Abilities = CreateDefaultSubobject<UAbilityComponent>(TEXT("Abilities"));
	SoftTarget = CreateDefaultSubobject<USoftTargetComponent>(TEXT("SoftTarget"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

}

bool ACharacterBase::TryAttack(bool bHeavy)
{
	if (!ActionState || !Combo)
	{		
		UE_LOG(LogTemp, Warning, TEXT("TryAttackFail_01"));

		return false;
	}

	if (!Combo->IsAttacking() && !ActionState->CanEnterState(ECharacterActionState::Attacking))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAttackFail_02"));

		return false;
	}

	if (!Combo->RequestAttack(bHeavy))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAttackFail_03"));

		return false;
	}

	ActionState->TryEnterState(ECharacterActionState::Attacking);
	return true;
}

bool ACharacterBase::TryCastAbility(int32 Slot)
{
	if (!ActionState || !Abilities)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryCastFail_01"));
		return false;
	}

	if (!ActionState->CanEnterState(ECharacterActionState::Casting))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryCastFail_02"));
		return false;
	}

	if (!Abilities->TryActivateAbility(Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryCastFail_03"));
		return false;
	}

	ActionState->TryEnterState(ECharacterActionState::Casting);
	return true;
}

bool ACharacterBase::TryDodge(FVector Direction)
{
	return false;
}

void ACharacterBase::HandleDeath()
{
	if (ActionState)
	{
		ActionState->ForceEnterState(ECharacterActionState::Dead);
	}

	 GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	 GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	 GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	 GetMesh()->SetSimulatePhysics(true);
	 //Apply impulse maybe, animation movement might be enough
	 UE_LOG(LogTemp, Warning, TEXT("DEATH"));
	 //When Death Animtion or Some sort of death effect ends that should destory the actor

	 GetCharacterMovement()->DisableMovement();

	 if (Equipment)
	 {
		 Equipment->UnequipWeapon();
	 }


	 SetLifeSpan(3.0f);
}

void ACharacterBase::HandleStagger(float PoiseDamage)
{
	//Stagger handling eventually
}

FVector ACharacterBase::GetPreferredTargetDirection() const
{
	return GetActorForwardVector(); 
}

UAttributeSet* ACharacterBase::GetAttributeSet() 
{
	return AttributeSet;
}

UActionStateComponent* ACharacterBase::GetActionState()
{
	return ActionState;
}

UMotionWarpingComponent* ACharacterBase::GetMotionWarping()
{
	return MotionWarping;
}

USoftTargetComponent* ACharacterBase::GetSoftTarget()
{
	return SoftTarget;
}

UComboComponent* ACharacterBase::GetCombo()
{
	return Combo;
}

int32 ACharacterBase::GetTeamID() const
{
	return TeamID;
}

float ACharacterBase::GetOrbitAngleOffset() const
{
	return OrbitAngleOffset;
}

void ACharacterBase::SetOrbitAngleOffset(float NewOffset)
{
	OrbitAngleOffset = NewOffset;
	bOrbitAngleAssigned = true;
}

void ACharacterBase::HandleAttributeDepleted(FGameplayTag AttributeTag)
{
	if (AttributeTag == HealthAttributeTag)
	{
		HandleDeath();
	}
}

void ACharacterBase::HandleActionStateChanged(ECharacterActionState OldState, ECharacterActionState NewState)
{
	if (OldState == ECharacterActionState::Attacking && NewState != ECharacterActionState::Attacking)
	{
		if (Combo)
		{
			Combo->ResetCombo();
		}
	}

	if (OldState == ECharacterActionState::Casting && NewState != ECharacterActionState::Casting)
	{
		if (Abilities)
		{
			Abilities->CancelCast();
		}
	}
}


// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (AttributeSet)
	{
		AttributeSet->OnAttributeDepleted.AddDynamic(this, &ACharacterBase::HandleAttributeDepleted);
	}

	if (ActionState)
	{
		ActionState->OnActionStateChanged.AddDynamic(this, &ACharacterBase::HandleActionStateChanged);
	}

	if (StartingWeapon && Equipment)
	{
		if (UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>())
		{
			FGuid ID = Inventory->AddWeapon(StartingWeapon, 0);
			if (FWeaponInstance* StartingWeaponRef = Inventory->FindWeapon(ID))
			{
				Equipment->EquipWeapon(*StartingWeaponRef);
			}
		}

		else
		{
			FWeaponInstance StartingWeaponInstance;
			StartingWeaponInstance.Definition = StartingWeapon;
			StartingWeaponInstance.UpgradeLevel = 0;
			StartingWeaponInstance.InstanceID = FGuid::NewGuid();

			Equipment->EquipWeapon(StartingWeaponInstance);
		}
	}
	if(!bOrbitAngleAssigned)
	{
		OrbitAngleOffset = FMath::FRandRange(0.0f, 360.0f);
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

