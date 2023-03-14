// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EncounterTracker.generated.h"

class UHorizontalBox;
class UTurn;
class UTurnTracker;
class UTextBlock;
class AEncounter;

/**
 * 
 */
UCLASS()
class RPG_API UEncounterTracker : public UUserWidget
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UHorizontalBox* TurnTrackerContainer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UTextBlock* CharacterName;
	
	TArray<UTurn*> Turns;

	TWeakObjectPtr<AEncounter> Encounter;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UTurnTracker> TurnTrackerClass;

	UFUNCTION()
	void OnCurrentTurnChanged(UTurn* Turn);

	UFUNCTION()
	void OnEncounterEnd(AEncounter* InEncounter);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterJoinEncounter(ARPGCharacter* JoiningCharacter, AEncounter* InEncounter);

public:
	void SetupEncounter(AEncounter* InEncounter);

	void AddTurnsToTracker(TArray<UTurn*> InTurns);

	UFUNCTION(BlueprintCallable)
	void AddTurnToTracker(UTurn* InTurn);
};
