// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "WeaponDefinition.h"
#include "AttributeSet.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


TArray<FGuid> UInventoryComponent::GetAllWeaponIDs() const
{
	TArray<FGuid> AllWeaponIDs;

	for (const FWeaponInstance& W : Weapons)
	{
		AllWeaponIDs.Add(W.InstanceID);
	}
	
	return AllWeaponIDs;
}

bool UInventoryComponent::GetWeaponInfo(FGuid InstanceID, FWeaponInstance& OutWeapon) const
{
	const FWeaponInstance* Found = Weapons.FindByPredicate([InstanceID](const FWeaponInstance& W) { return W.InstanceID == InstanceID; });

	if (!Found)
	{
		OutWeapon = FWeaponInstance();
		return false;
	}

	OutWeapon = *Found;
	return true;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	AttributeSet = GetOwner()->FindComponentByClass<UAttributeSet>();

}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FGuid UInventoryComponent::AddWeapon(UWeaponDefinition* Definition, int32 StartingUpgradeLevel)
{
	if (!Definition)
	{
		return FGuid();
	}

	FWeaponInstance NewInstance;
	NewInstance.Definition = Definition;
	NewInstance.UpgradeLevel = FMath::Clamp(StartingUpgradeLevel, 0, Definition->MaxUpgradeLevel);
	NewInstance.InstanceID = FGuid::NewGuid();

	Weapons.Add(NewInstance);
	OnInventoryChanged.Broadcast(NewInstance.InstanceID);

	return NewInstance.InstanceID;
}

FWeaponInstance* UInventoryComponent::FindWeapon(FGuid InstanceID)
{
	return Weapons.FindByPredicate([InstanceID](const FWeaponInstance& W) {return W.InstanceID == InstanceID;});
}

bool UInventoryComponent::RemoveWeapon(FGuid InstanceID)
{
	const int32 Removed = Weapons.RemoveAll([InstanceID](const FWeaponInstance& W) {return W.InstanceID == InstanceID; });

	if (Removed > 0)
	{
		OnInventoryChanged.Broadcast(InstanceID);
		return true;
	}
	return false;
}

bool UInventoryComponent::UpgradeWeapon(FGuid InstanceID)
{
	float Cost = 0.0f;
	if (!CanUpgradeWeapon(InstanceID,Cost))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory_Fail1"));
		return false;
	}

	FWeaponInstance* WeaponInstance = FindWeapon(InstanceID);
	FInstantChange InstantChangeStruct;
	InstantChangeStruct.AttributeTag = CurrencyAttributeTag;
	InstantChangeStruct.Delta = -Cost;
	
	if (!AttributeSet->ApplyInstantChange(InstantChangeStruct))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory_Fail2"));
		return false;
	}
	WeaponInstance->UpgradeLevel++;
	OnWeaponUpgraded.Broadcast(InstanceID);
	UE_LOG(LogTemp, Warning, TEXT("Inventory_Success1"));

	return true;
}

bool UInventoryComponent::CanUpgradeWeapon(FGuid InstanceID, float& OutCost) const
{
	const FWeaponInstance* WeaponInstance = Weapons.FindByPredicate([InstanceID](const FWeaponInstance& W) {return W.InstanceID == InstanceID; });
	OutCost = 0.0f;
	if (!WeaponInstance)
	{
		return false;
	}

	UWeaponDefinition* WeaponDefinition = WeaponInstance->Definition;

	if (!WeaponDefinition)
	{
		return false;
	}

	if (WeaponInstance->UpgradeLevel >= WeaponDefinition->MaxUpgradeLevel)
	{
		return false;
	}

	if (!WeaponDefinition->UpgradeCostCurve)
	{
		return false;
	}


	OutCost = WeaponDefinition->UpgradeCostCurve->GetFloatValue(WeaponInstance->UpgradeLevel);
	if (!AttributeSet)
	{
		return false;
	}

	if (AttributeSet->GetAttributeValue(CurrencyAttributeTag) < OutCost)
	{
		return false;
	}

	return true;
}

