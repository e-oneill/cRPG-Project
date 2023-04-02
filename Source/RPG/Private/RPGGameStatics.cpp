// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameStatics.h"
#include "../Public/GameFramework/RPGGameState.h"

UEncounterManager* URPGGameStatics::GetEncounterManager(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	ARPGGameState GS = World->GetGameState<ARPGGameState>();
	if (!GameState)
	{
		return nullptr;
	}

	UEncounterManager* EncounterManager = GameState->GetEncounterManager();

	return EncounterManager;
}
