// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameMode.h"
#include "../RPG.h"
#include "RPGPlayerController.h"
#include "RPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/RPGGameState.h"

ARPGGameMode::ARPGGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARPGPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	RandomSeed = rand();
}

ARPGPlayerController* ARPGGameMode::SetupControlledActor(UPlayerControlComponent* PlayerControlComp)
{
	TArray<APlayerState*> Players = GetPlayerArray();
	APlayerState* PlayerState;
	if (Players.Num() > PlayerControlComp->GetPlayerIndex() + 1)
	{
		PlayerState = Players[PlayerControlComp->GetPlayerIndex()];
		
	} 
	//the array does not contain the player who controls this character, add them to another character and keep track of who was meant to control them. Assign characters to player 1
	else
	{
		PlayerState = Players[0];
	}

	ARPGPlayerController* PlayerController = Cast<ARPGPlayerController>(PlayerState->GetPlayerController());
	if (!PlayerController)
	{
		UE_LOG(LogRPG, Error, TEXT("Player %s does not have a Player Controller that is a subclass of ARPGPlayerController"), *PlayerState->GetName());
		return nullptr;
	}
	AssignControlledActorToPlayer(PlayerControlComp, PlayerController);

	ARPGGameState* RPGGameState = GetGameState<ARPGGameState>();
	if (RPGGameState)
	{
		RPGGameState->AddToControlledActors(PlayerControlComp);
	}

	return PlayerController;

}

void ARPGGameMode::StartPlay()
{
	Super::StartPlay();
	RandomStream = FRandomStream(RandomSeed);
	//RandomStream.Initialize(RandomSeed);
}

int ARPGGameMode::GetRandomInt(int lower, int higher)
{
	return RandomStream.RandRange(lower, higher);
}

TArray<APlayerState*> ARPGGameMode::GetPlayerArray()
{
	return GetGameState<AGameStateBase>()->PlayerArray;
}

void ARPGGameMode::AssignControlledActorToPlayer(UPlayerControlComponent* ControlledActor, ARPGPlayerController* Player)
{
	Player->AddToControlled(ControlledActor);
	ControlledActor->SetPlayerController(Player);
}

void ARPGGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (!Cast<ARPGPlayerController>(NewPlayer))
	{
		return;
	}

	//when a new player joins, see if there are any characters who should have been controlled by that player
	//while this is a 2-tier loop, the number of entries should be 4, so it will run max 4 times.
	TArray<APlayerState*> Players = GetPlayerArray();
	if (Players.Num() == 1)
	{
		//first player is joining, no point checking
		return;
	}

	int thisPlayer = Players.Num() - 1;
	ARPGGameState* RPGGameState = GetGameState<ARPGGameState>();
	if (RPGGameState)
	{
		//get the array of all player controlled actors from the game state
		TArray<UPlayerControlComponent*> Controlled = RPGGameState->GetControlledActors();
		for (int i = 0; i < Controlled.Num(); i++)
		{
			//if this control component should be controlled by this player, we will switch its controller
			if (Controlled[i]->GetPlayerIndex() == thisPlayer)
			{
				if (Controlled[i]->GetPlayerController())
				{
					Controlled[i]->GetPlayerController()->RemoveFromControlled(Controlled[i]);
				}
				AssignControlledActorToPlayer(Controlled[i], Cast<ARPGPlayerController>(NewPlayer));
			}
		}
	}



}
