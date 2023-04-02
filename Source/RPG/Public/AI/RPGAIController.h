// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/GameplayActionSystemStatics.h"
#include "PlanEntry.h"
#include "RPGAIController.generated.h"

class UGameplayActionComponent;
class UGOAPPlanningComponent;
class UAction;
class UTurn;

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

	float GetTargetAwareness() const { return TargetAwareness; }
	void SetTargetAwareness(float val) { TargetAwareness = val; }
protected:
	UPROPERTY(BlueprintReadWrite)
	FVector LastSpottedLocation;

	UPROPERTY(BlueprintReadWrite)
	float LastSpottedTime;

	UPROPERTY(BlueprintReadWrite)
	float TargetAwareness;
};

/**
 * 
 */
UCLASS()
class RPG_API ARPGAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARPGAIController();

	void AIMoveToLocation(FVector Location);

	bool HasSpottedCharacter(UGameplayActionComponent* SpottedActionComponent);

	FSpottedCharacter& AddOrUpdateSpottedCharacter(UGameplayActionComponent* SpottedActionComponent);

	UGOAPPlanningComponent* GetGOAPPlanner() const { return GOAPPlanner; }
	void SetGOAPPlanner(UGOAPPlanningComponent* val) { GOAPPlanner = val; }
	TArray<FSpottedCharacter> GetSpottedCharacters() const { return SpottedCharacters; }
	void SetSpottedCharacters(TArray<FSpottedCharacter> val) { SpottedCharacters = val; }
	TArray<UAction*> GetActions();

	UPlayerControlComponent* GetPlayerControlComponent() const { return PlayerControlComponent; }
	void SetPlayerControlComponent(UPlayerControlComponent* val) { PlayerControlComponent = val; }

	void GetPlanAndExecute();


protected:
	UPlayerControlComponent* PlayerControlComponent;
	
	UGameplayActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere)
	UGOAPPlanningComponent* GOAPPlanner;

	UTurn* Turn;

	TArray<FPlanEntry> ActionPlan;

	UPROPERTY(BlueprintReadWrite)
	float Alertness;

	float AlertnessTickRate = .25f;

	UPROPERTY(BlueprintReadOnly)
	TArray<FSpottedCharacter> SpottedCharacters;

	void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	void HandlePawnSpotted(APawn* SeenPawn);
	UFUNCTION()
	void HandleNoiseHeard(APawn* InInstigator, const FVector& Location, float Volume);

	void StartEncounterWithSpottedCharacter(UGameplayActionComponent* SpottedCharacter);

	void JoinEncountersWithSpottedCharacter(UGameplayActionComponent* SpottedCharacter);

	void ExecuteNextAction(TArray<FPlanEntry>& Plan);

	bool ExecutePlanEntry(FPlanEntry& PlanEntry);

	void HandleFinishPlan();

	UFUNCTION()
	void OnTurnAssigned(UTurn* NewTurn);
	UFUNCTION()
	void OnTurnStart(UTurn* NewTurn);
	UFUNCTION()
	void OnActionComplete(UAction* CompletedAction, EActionState State, EActionState OldState);




};
