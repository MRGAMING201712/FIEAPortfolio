// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponActor.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, FWeaponInstance, NewInstance);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnWeaponEquipped OnWeaponEquipped;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipWeapon(const FWeaponInstance& WeaponInstance);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipWeapon();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AWeaponActor* GetEquippedWeaponActor() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool GetEquippedWeaponInstance(FWeaponInstance& OutInstance) const;

	UFUNCTION()
	void HandleWeaponUpgraded(FGuid InstanceID);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<AWeaponActor> EquippedWeaponActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	FWeaponInstance EquippedWeaponInstance;
};
