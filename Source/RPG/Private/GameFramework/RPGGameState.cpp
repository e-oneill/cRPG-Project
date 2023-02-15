// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RPGGameState.h"
#include "GameState/EncounterManager.h"
#include "UObject/Object.h"
#include "Kismet/GameplayStatics.h"
#include "../RPGCharacter.h"
#include "GameState/Encounter.h"
#include "Net/UnrealNetwork.h"
#include "Math/RandomStream.h"

FRandomStream ARPGGameState::RandomStream;

ARPGGameState::ARPGGameState() : Super()
{
	EncounterManager = CreateDefaultSubobject<UEncounterManager>(TEXT("EncounterManager"));

	RandomSeed = 1337;
}

void ARPGGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();
	//FRandomStream RandomStream(GetRandomSeed());
	RandomStream.Initialize(RandomSeed);
}

TArray<ARPGCharacter*> ARPGGameState::GetPlayerCharacters()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARPGCharacter::StaticClass(), Actors);
	TArray<ARPGCharacter*> Characters;
	for (AActor* Actor : Actors)
	{
		Characters.Add(Cast<ARPGCharacter>(Actor));
	}
	return Characters;
}

void ARPGGameState::EndTurnForCharacter_Implementation(ARPGCharacter* InCharacter)
{
	InCharacter->Server_EndCharacterTurn();
}

void ARPGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGGameState, EncounterManager);
}

int32 ARPGGameState::GetRandomIntInRange(int32 Min, int32 Max)
{
	return RandomStream.RandRange(Min, Max);
}

float ARPGGameState::GetRandomFloatInRange(float Min, float Max)
{
	
	return RandomStream.FRandRange(Min, Max);
}

FVector ARPGGameState::GetRandomPointInCone(FVector Direction, float ConeAngle)
{
	return RandomStream.VRandCone(Direction, ConeAngle);
}

FVector ARPGGameState::GetRandomPointInBox(FVector Min, FVector Max)
{	
	FBox Box = FBox(Min, Max);
	return RandomStream.RandPointInBox(Box);
}
