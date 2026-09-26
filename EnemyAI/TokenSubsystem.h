// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TokenSubsystem.generated.h"



UENUM(BlueprintType)
enum class ETokenType : uint8
{
	Melee,
	Ranged
};


USTRUCT()
struct FTokenPool
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<TObjectPtr<AActor>> Holders;

	UPROPERTY()
	int32 MaxTokens = 2;
};


/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UTokenSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Tokens")
	bool RequestToken(AActor* Requester, ETokenType TokenType);

	UFUNCTION(BlueprintCallable, Category = "Tokens")
	void ReleaseToken(AActor* Requester, ETokenType TokenType);

	UFUNCTION(BlueprintPure, Category = "Tokens")
	bool HasToken(AActor* Requester, ETokenType TokenType) const;

	UFUNCTION(BlueprintPure, Category = "Tokens")
	bool HasFreeToken(ETokenType TokenType) const;

	UFUNCTION(BlueprintCallable, Category = "Tokens")
	void SetMaxTokens(ETokenType TokenType, int32 NewMax);

	// Called from HandleDeath as a stop, clears every pool
	UFUNCTION(BlueprintCallable, Category = "Tokens")
	void ReleaseAllTokens(AActor* Requester);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void PrunePool(FTokenPool& Pool);

	UPROPERTY()
	TMap<ETokenType, FTokenPool> Pools;

};
