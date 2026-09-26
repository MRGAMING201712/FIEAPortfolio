// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{ 
	//New Collision Settings for capsule and the mesh so they do not affect the camera
	GetCharacterMovement()->bUseRVOAvoidance = true;
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::HandleDeath()
{
	if (!AttributeSet)
	{
		return;
	}
	float XP = AttributeSet->GetAttributeValue(XPTag);
	float Souls = AttributeSet->GetAttributeValue(SoulsTag);
	OnEnemyDeath.Broadcast(XP, Souls);

	Super::HandleDeath();
}

void AEnemyCharacter::ApplyDifficultyModifiers_Implementation(const TMap<FGameplayTag, float>& Modifiers)
{
	if (!AttributeSet)
	{
		return; 
	}

	for (const TPair<FGameplayTag, float>& Pair : Modifiers)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Tag: %s"), *Pair.Key.ToString());
		AttributeSet->ApplyModifier(
			Pair.Key,                              
			EModifierOperation::Multiplicative,
			Pair.Value, //ex 0.5 = +50%
			0.0f, //0 = permanent
			this,
			ERecalcContext::LevelUp);
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Could switch to (360/enemiesinwave)*spawnindex; which would make enemies orbit uniformly
	OrbitAngleOffset = FMath::FRandRange(0.0f, 360.0f);
}
