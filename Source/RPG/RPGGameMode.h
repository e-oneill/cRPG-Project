// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGGameMode.generated.h"

class ARPGPlayerController;
class UPlayerControlComponent;
class APlayerState;

UCLASS(minimalapi)
class ARPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARPGGameMode();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	ARPGPlayerController* SetupControlledActor(UPlayerControlComponent* PlayerControlComp);

	void StartPlay() override;

	int GetRandomInt(int lower, int higher);

protected:
	TArray<APlayerState*> GetPlayerArray();

	void AssignControlledActorToPlayer(UPlayerControlComponent* ControlledActor, ARPGPlayerController* Player);

	FRandomStream RandomStream;

	int32 RandomSeed;

};



