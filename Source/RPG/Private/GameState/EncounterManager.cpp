// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/EncounterManager.h"
#include "../RPG.h"
#include "GameState/Encounter.h"
#include "../RPGCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/RPGGameState.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

extern TAutoConsoleVariable<bool> CVarDebugAll;
bool EM_DebugAll = CVarDebugAll.GetValueOnGameThread();

extern TAutoConsoleVariable<bool> CVarDebugGameState;
bool EM_DebugGameState = CVarDebugGameState.GetValueOnGameThread();

// Sets default values for this component's properties
UEncounterManager::UEncounterManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UEncounterManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEncounterManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AEncounter* UEncounterManager::StartEncounterWithPlayerCharacters()
{
	ARPGGameState* GameState = Cast<ARPGGameState>(GetOwner());
	if (GameState)
	{
		TArray<ARPGCharacter*> Characters = GameState->GetPlayerCharacters();
		return StartEncounter(Characters);
	}
	return nullptr;
}

AEncounter* UEncounterManager::StartEncounter(TArray<ARPGCharacter*> Characters)
{
	if (Characters.Num() < 1)
	{
		return nullptr;
	}
	//Create an encounter actor
	FActorSpawnParameters SpawnParams;
	AEncounter* NewEncounter = GetWorld()->SpawnActor<AEncounter>(AEncounter::StaticClass(), Characters[0]->GetOwner()->GetActorLocation(), Characters[0]->GetOwner()->GetActorRotation(), SpawnParams);
	if (!NewEncounter)
	{
		return nullptr;
	}
	NewEncounter->AddCharacters(Characters);
	NewEncounter->Start();
	Encounters.Add(NewEncounter);
	NewEncounter->AddEncounterToUI();


	return NewEncounter;
}



AEncounter* UEncounterManager::StartEncounter(TArray<UGameplayActionComponent*> InComponents)
{
	if (InComponents.Num() < 1)
	{
		return nullptr;
	}
	FActorSpawnParameters SpawnParams;
	AEncounter* NewEncounter = GetWorld()->SpawnActor<AEncounter>(AEncounter::StaticClass(), InComponents[0]->GetOwner()->GetActorLocation(), InComponents[0]->GetOwner()->GetActorRotation(), SpawnParams);
	if (!NewEncounter)
	{
		return nullptr;
	}
	NewEncounter->AddActionComponents(InComponents);
	NewEncounter->Start();
	Encounters.Add(NewEncounter);
	NewEncounter->AddEncounterToUI();
	return NewEncounter;
}

void UEncounterManager::EndEncounter(AEncounter* InEncounter)
{
	InEncounter->End();

	Encounters.Remove(InEncounter);
	InEncounter->Destroy();
}

void UEncounterManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEncounterManager, Encounters);
}

