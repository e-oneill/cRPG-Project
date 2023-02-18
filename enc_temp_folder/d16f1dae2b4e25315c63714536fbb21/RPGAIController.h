// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "RPGAIController.generated.h"

class UGameplayActionComponent;

USTRUCT(Blueprintable, BlueprintType)
struct FSpottedCharacter
{
	GENERATED_BODY();

public:
	/*FSpottedCharacter()
	{

	}

	FSpottedCharacter(FSpottedCharacter& InSpottedCharacter)
	{
		SpottedActionComponent = InSpottedCharacter.SpottedActionComponent;
		LastSpottedLocation = InSpottedCharacter.GetLastSpottedLocation();
		LastSpottedTime = InSpottedCharacter.GetLastSpottedTime();
	}

	FSpottedCharacter(UGameplayActionComponent* InActionComponent)
	{
		SpottedActionComponent = InActionComponent;
		UpdateSpottedCharacter();
	}

	FSpottedCharacter(UGameplayActionComponent* InActionComponent, FVector InLocation, float InTime)
	{
		SpottedActionComponent = InActionComponent;
		LastSpottedLocation = InLocation;
		LastSpottedTime = InTime;
	}*/

	UPROPERTY(BlueprintReadOnly)
	UGameplayActionComponent* SpottedActionComponent;

	void UpdateSpottedCharacter()
	{
		LastSpottedLocation = SpottedActionComponent->GetOwner()->GetActorLocation();
		LastSpottedTime = SpottedActionComponent->GetWorld()->GetTimeSeconds();
	}

	FVector GetLastSpottedLocation() const { return LastSpottedLocation; }
	float GetLastSpottedTime() const { return LastSpottedTime; }

protected:
	UPROPERTY(BlueprintReadWrite)
	FVector LastSpottedLocation;

	UPROPERTY(BlueprintReadWrite)
	float LastSpottedTime;
};

/**
 * 
 */
UCLASS()
class RPG_API ARPGAIController : public AAIController
{
	GENERATED_BODY()

public:
	void AIMoveToLocation(FVector Location);

	bool HasSpottedCharacter(UGameplayActionComponent* SpottedActionComponent);

	void AddOrUpdateSpottedCharacter(UGameplayActionComponent* SpottedActionComponent);

protected:
	TArray<FSpottedCharacter> SpottedCharacters;

	void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	void HandlePawnSpotted(APawn* SeenPawn);
	UFUNCTION()
	void HandleNoiseHeard(APawn* InInstigator, const FVector& Location, float Volume);



};
