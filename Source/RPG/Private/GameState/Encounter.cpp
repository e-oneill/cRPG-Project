// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/Encounter.h"
#include "../RPGCharacter.h"
#include "GameState/Turn.h"
#include "../RPG.h"
#include "../RPGPlayerController.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameFramework/RPGGameState.h"
#include "ActionSystemTags.h"

extern TAutoConsoleVariable<bool> CVarDebugAll;
bool Enc_DebugAll = CVarDebugAll.GetValueOnGameThread();

extern TAutoConsoleVariable<bool> CVarDebugGameState;
bool Enc_DebugGameState = CVarDebugGameState.GetValueOnGameThread();

// Sets default values
AEncounter::AEncounter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->bReplicates = true;
	this->bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AEncounter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEncounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEncounter::Start()
{
	CurrentTurnIndex = 0;
	Turns[0]->StartTurn();
	
	NetMulticast_OnEncounterStart();
}

void AEncounter::EndCurrentTurn()
{
	CurrentTurn->EndTurn();
	CurrentTurnIndex = ++CurrentTurnIndex % Turns.Num();
	
	Turns[CurrentTurnIndex]->StartTurn();
	OnCurrentTurnChanged.Broadcast(CurrentTurn);
}

void AEncounter::End()
{
	for (int i = 0; i < Turns.Num(); i++)
	{
		
		Turns[i]->GetActionComponent()->SetTurn(nullptr);
		Turns[i]->Destroy();
	}

	NetMulticast_OnEncounterEnd();

	//Destroy();
}

void AEncounter::AddCharacters(TArray<ARPGCharacter*> InCharacters)
{
	for (ARPGCharacter* InCharacter : InCharacters)
	{
		AddCharacterToEncounter(InCharacter);
	}
}

void AEncounter::AddActionComponents(TArray<UGameplayActionComponent*> ActionComponents)
{
	for (UGameplayActionComponent* ActionComponent : ActionComponents)
	{
		AddCharacterToEncounter(ActionComponent);
	}
}

bool AEncounter::IsCharacterInEncounter(ARPGCharacter* Character)
{
	return IsCharacterInEncounter(Character->GetActionComponent());
	/*for (UTurn* Turn : Turns)
	{
		ARPGCharacter* TurnChar = Cast<ARPGCharacter>(Turn->GetActionComponent()->GetOwner());
		if (TurnChar && TurnChar == Character)
		{
			return true;
		}
	}
	return false;*/
}

bool AEncounter::IsCharacterInEncounter(UGameplayActionComponent* ActionComponent)
{
	for (UTurn* Turn : Turns)
	{
		if (Turn->GetActionComponent() == ActionComponent)
		{
			return true;
		}
	}
	return false;
}

void AEncounter::AddCharacterToEncounter(ARPGCharacter* InCharacter)
{
	AddCharacterToEncounter(InCharacter->GetActionComponent());
}

void AEncounter::AddCharacterToEncounter(UGameplayActionComponent* InActionComponent)
{
	if (IsCharacterInEncounter(InActionComponent))
	{
		if (Enc_DebugAll || Enc_DebugGameState)
			UE_LOG(LogRPG, Log, TEXT("Tried to add %s to encounter, but they were already in the encounter"), *InActionComponent->GetOwner()->GetName());

		return;
	}

	FString OwnerName = InActionComponent->GetOwner()->GetName();
	FName TurnName = FName(FString("Turn_") + OwnerName);
	


	UTurn* Turn = NewObject<UTurn>(this, UTurn::StaticClass(), TurnName);
	Turn->InitializeTurn(InActionComponent, this);
	Turns.Add(Turn);
	NetMulticast_OnCharacterJoinEncounter(Cast<ARPGCharacter>(InActionComponent->GetOwner()));
}

void AEncounter::RemoveCharacterFromEncounter(ARPGCharacter* InCharacter)
{
	UGameplayActionComponent* CharActionComponent = InCharacter->GetActionComponent();
	RemoveCharacterFromEncounter(CharActionComponent);
}

void AEncounter::RemoveCharacterFromEncounter(UGameplayActionComponent* InActionComponent)
{
	int Index = -1;
	for (int i = 0; i < Turns.Num(); i++)
	{
		if (Turns[i]->GetActionComponent() == InActionComponent)
		{
			NetMulticast_OnCharacterLeftEncounter(Cast<ARPGCharacter>(InActionComponent->GetOwner()));
			InActionComponent->SetTurn(nullptr);
			Index = i;
			break;
		}
	}

	if (Index > -1)
	{
		UTurn* TurnToRemove = Turns[Index];
		Turns.RemoveAt(Index);
		TurnToRemove->Destroy();
	}

	if (CheckShouldEncounterEnd())
	{
		End();
	}
}

bool AEncounter::CheckShouldEncounterEnd()
{
	ARPGGameState* GameState = GetWorld()->GetGameState<ARPGGameState>();
	FGameplayTag PlayerFaction = FActionSystemTags::Get().Player_Faction;

	//if there are any hostile chars left, return false
	for (UTurn* Turn : Turns)
	{
		if (GameState->IsFactionHostile(PlayerFaction, Turn->GetActionComponent()->GetFaction()))
		{
			return false;
		}
	}

	return true;
}

void AEncounter::NetMulticast_OnCharacterJoinEncounter_Implementation(ARPGCharacter* InCharacter)
{
	InCharacter->OnEncounterJoined.Broadcast(InCharacter, this);
	OnCharacterJoinEncounter.Broadcast(InCharacter, this);
}

void AEncounter::NetMulticast_OnCharacterLeftEncounter_Implementation(ARPGCharacter* InCharacter)
{
	InCharacter->OnEncounterLeft.Broadcast(InCharacter, this);
	OnCharacterLeftEncounter.Broadcast(InCharacter, this);
}

void AEncounter::NetMulticast_OnEncounterStart_Implementation()
{
	OnEncounterStart.Broadcast(this);
}

void AEncounter::NetMulticast_OnEncounterEnd_Implementation()
{
	OnEncounterEnd.Broadcast(this);
}

void AEncounter::OnRep_CurrentTurn()
{
	OnCurrentTurnChanged.Broadcast(CurrentTurn);
}

void AEncounter::AddEncounterToUI()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No World"));
	}
	ARPGPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ARPGPlayerController>();
	PlayerController->AddEncounterToUI(this);
}

void AEncounter::AddTurn(UTurn* InTurn)
{
	Turns.Add(InTurn);
	InTurn->SetEncounter(this);
	NetMulticast_OnCharacterJoinEncounter(Cast<ARPGCharacter>(InTurn->GetActionComponent()->GetOwner()));
}

void AEncounter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEncounter, Turns);
	DOREPLIFETIME(AEncounter, CurrentTurn);
	DOREPLIFETIME(AEncounter, CurrentTurnIndex);


}

bool AEncounter::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);


	WroteSomething |= Channel->ReplicateSubobject(CurrentTurn, *Bunch, *RepFlags);
	WroteSomething |= Channel->ReplicateSubobjectList(Turns, *Bunch, *RepFlags);
	return WroteSomething;
}

