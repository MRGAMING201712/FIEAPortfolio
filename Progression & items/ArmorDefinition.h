// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "ArmorDefinition.generated.h"

UENUM (BlueprintType)
enum class EArmorSlot : uint8
{
	Head,
	Chest,
	Hands,
	Legs,
	Ring1,
	Ring2,
	Ring3
};

/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UArmorDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor Definition")
	EArmorSlot SlotToUse = EArmorSlot::Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor Definition")
	TObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor Definition")
	TArray<FAttributeModifier> ModifiersToGrant;

};
