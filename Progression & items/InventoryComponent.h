// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventoryComponent.generated.h"

class UWeaponDefinition;
class UArmorDefinition;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, FGuid, InstanceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUpgraded, FGuid, InstanceID);

USTRUCT(BlueprintType)
struct FWeaponInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWeaponDefinition> Definition;

	UPROPERTY(BlueprintReadOnly)
	int32 UpgradeLevel; //0 up to definition's MaxUpgradeLevel

	UPROPERTY(BlueprintReadOnly)
	FGuid InstanceID;
};

USTRUCT(BlueprintType)
struct FArmorInstance
{
	GENERATED_BODY()

	TObjectPtr<UArmorDefinition> Definition;

	FGuid InstanceID;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponUpgraded OnWeaponUpgraded;

	UPROPERTY()
	TArray<FWeaponInstance> Weapons;

	UPROPERTY()
	TArray<FArmorInstance> Armor;

	UFUNCTION(BlueprintCallable)
	FGuid AddWeapon(UWeaponDefinition* Definition, int32 StartingUpgradeLevel = 0);

	FWeaponInstance* FindWeapon(FGuid InstanceID);

	UFUNCTION(BlueprintCallable)
	bool RemoveWeapon(FGuid InstanceID);

	UFUNCTION(BlueprintCallable)
	bool UpgradeWeapon(FGuid InstanceID);

	UFUNCTION(BlueprintPure)
	bool CanUpgradeWeapon(FGuid InstanceID, float& OutCost) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FGameplayTag CurrencyAttributeTag;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FGuid> GetAllWeaponIDs() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetWeaponInfo(FGuid InstanceID, FWeaponInstance& OutWeapon) const;
	 
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
