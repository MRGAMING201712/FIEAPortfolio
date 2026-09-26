// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State Tree Component"));

	if (StateTreeComponent)
	{
		StateTreeComponent->SetStartLogicAutomatically(false);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//State Tree from enemy class
	if(AEnemyCharacter* PossessedEnemy = Cast<AEnemyCharacter>(InPawn))
	{
		if (PossessedEnemy->EnemyStateTree && StateTreeComponent)
		{
			StateTreeComponent->SetStateTree(PossessedEnemy->EnemyStateTree);
			StateTreeComponent->StartLogic();
		}
	}
}
