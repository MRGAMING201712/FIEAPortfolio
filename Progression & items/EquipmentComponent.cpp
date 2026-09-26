// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "WeaponDefinition.h"
#include "InventoryComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool UEquipmentComponent::EquipWeapon(const FWeaponInstance& WeaponInstance)
{
	if (!WeaponInstance.Definition)
	{
		return false;
	}

	if (EquippedWeaponActor)
	{
		EquippedWeaponActor->Destroy();
		EquippedWeaponActor = nullptr;
	}

	if (!WeaponInstance.Definition->WeaponActorClass)
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponActor* NewWeapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponInstance.Definition->WeaponActorClass, FTransform::Identity, SpawnParams);

	if (!NewWeapon)
	{
		return false;
	}

	NewWeapon->InitializeFromInstance(WeaponInstance);

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		NewWeapon->Destroy();
		return false;
	}

	NewWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponInstance.Definition->EquippedSocketName);

	NewWeapon->SetActorRelativeTransform(WeaponInstance.Definition->AttachOffset);

	EquippedWeaponActor = NewWeapon;
	EquippedWeaponInstance = WeaponInstance;
	OnWeaponEquipped.Broadcast(WeaponInstance);
	return true;
}

void UEquipmentComponent::UnequipWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("UNEQUIPEMTER"));

	if (EquippedWeaponActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNEQUIP IF STATEMENT"));
		EquippedWeaponActor->Destroy();
		EquippedWeaponActor = nullptr;
		EquippedWeaponInstance = FWeaponInstance();
		OnWeaponEquipped.Broadcast(FWeaponInstance());
	}
}

AWeaponActor* UEquipmentComponent::GetEquippedWeaponActor() const
{
	return EquippedWeaponActor;
}

bool UEquipmentComponent::GetEquippedWeaponInstance(FWeaponInstance& OutInstance) const
{
	OutInstance = EquippedWeaponInstance;
	return EquippedWeaponInstance.Definition != nullptr;
}

void UEquipmentComponent::HandleWeaponUpgraded(FGuid InstanceID)
{
	if (!Inventory)
	{
		return;
	}

	if (EquippedWeaponInstance.InstanceID != InstanceID)
	{
		return;   // not the one we're holding
	}

	// re-read from inventory and push down to the actor
	if (FWeaponInstance* Updated = Inventory->FindWeapon(InstanceID))
	{
		EquippedWeaponInstance = *Updated;
		if (EquippedWeaponActor)
		{
			EquippedWeaponActor->InitializeFromInstance(*Updated);
		}
	}
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (Inventory)
	{
		Inventory->OnWeaponUpgraded.AddDynamic(this, &UEquipmentComponent::HandleWeaponUpgraded);
	}
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

