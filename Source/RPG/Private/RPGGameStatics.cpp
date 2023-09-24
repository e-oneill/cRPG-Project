// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameStatics.h"
#include "../Public/GameFramework/RPGGameState.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

UEncounterManager* URPGGameStatics::GetEncounterManager(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	ARPGGameState* GameState = World->GetGameState<ARPGGameState>();
	if (!GameState)
	{
		return nullptr;
	}

	UEncounterManager* EncounterManager = GameState->GetEncounterManager();

	return EncounterManager;
}

ARPGPlayerController* URPGGameStatics::GetActionComponentPlayerController(UGameplayActionComponent* ActionComponent)
{
	AActor* Owner = ActionComponent->GetOwner();

	UPlayerControlComponent* PlayerControlComponent = Cast<UPlayerControlComponent>(Owner->GetComponentByClass(UPlayerControlComponent::StaticClass()));

	if (!PlayerControlComponent)
	{
		return nullptr;
	}

	return PlayerControlComponent->GetPlayerController();
}

ARPGGameState* URPGGameStatics::GetGameState(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	ARPGGameState* GameState = Cast<ARPGGameState>(World->GetGameState());

	return GameState;
}
