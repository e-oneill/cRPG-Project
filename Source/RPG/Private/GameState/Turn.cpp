// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/Turn.h"
#include "GameState/Encounter.h"
#include "Net/UnrealNetwork.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "ActionSystemTags.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "GameplayTagContainer.h"

extern TAutoConsoleVariable<bool> CVarDebugAll;
bool Turn_DebugAll = CVarDebugAll.GetValueOnGameThread();

extern TAutoConsoleVariable<bool> CVarDebugGameState;
bool Turn_DebugGameState = CVarDebugGameState.GetValueOnGameThread();

extern TAutoConsoleVariable<bool> CVarDebugNetworking;
bool Turn_DebugNetwork = CVarDebugNetworking.GetValueOnGameThread();



bool UTurn::IsCurrentTurn()
{
	if (!Encounter.IsValid())
	{	
		return false;
	}

	AEncounter* EncounterPointer = Encounter.Get();

	return EncounterPointer->GetCurrentTurn() == this;

}

void UTurn::InitializeTurn(UGameplayActionComponent* InActionComponent, TWeakObjectPtr<AEncounter> InEncounter)
{
	ActionComponent = InActionComponent;
	ActionComponent->SetTurn(this);
	Encounter = InEncounter;
	UsedMovement = 0.f;
	//get the movement attribute from the action component and set it.
	FGameplayTag MoveTag = FActionSystemTags::Get().Attr_Movement;
	UActionAttribute* MoveAttribute = ActionComponent->GetAttributeByTag(MoveTag);
	if (MoveAttribute)
	{
		Movement = MoveAttribute->GetAttributeValue();
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to add a turn to an actor %s who does not have the movement attribute. Movement will default to 0."), *InActionComponent->GetOwner()->GetName());
	}
	FGameplayTag APTag = FActionSystemTags::Get().Attr_ActionPoints;
	ActionPointAttribute = ActionComponent->GetAttributeByTag(APTag);
	if (ActionPointAttribute)
	{
		ActionPoints = ActionPointAttribute->GetAttributeValue();
		ActionPointAttribute->OnAttributeChanged.AddUniqueDynamic(this, &UTurn::OnActionPointsChanged);
	}

	MovementAttribute = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_Movement);
	if (MovementAttribute)
	{
		Movement = MovementAttribute->GetAttributeValue();
		MovementAttribute->OnAttributeChanged.AddUniqueDynamic(this, &UTurn::OnMovementAttributeChanged);
	}

}

void UTurn::OnActionPointsChanged(UGameplayActionComponent* OwningComponent, UActionAttribute* Attribute)
{
	UsedActionPoints = Attribute->GetAttributeBaseValue() - Attribute->GetAttributeValue();
}

void UTurn::OnMovementAttributeChanged(UGameplayActionComponent* OwningComponent, UActionAttribute* Attribute)
{
	UsedMovement = Attribute->GetAttributeBaseValue() - Attribute->GetAttributeValue();
}

void UTurn::StartTurn()
{
	if (Turn_DebugAll || Turn_DebugGameState)
	{
		UE_LOG(LogRPG, Log, TEXT("Starting turn..."));
	}
	Encounter->SetCurrentTurn(this);
	UsedMovement = 0.f;
	//reset action points
	ActionPoints = ActionPointAttribute->GetAttributeBaseValue();
	ActionPointAttribute->SetAttributeValue(ActionPointAttribute->GetAttributeBaseValue());
	MovementAttribute->SetAttributeValue(MovementAttribute->GetAttributeBaseValue());
	NetMulticast_OnTurnStart();
	
}

void UTurn::EndTurn()
{
	if (Turn_DebugAll || Turn_DebugGameState)
	{
		UE_LOG(LogRPG, Log, TEXT("Ending turn..."));
	}
	NetMulticast_OnTurnEnd();
}

void UTurn::RemoveFromEncounter()
{
	AEncounter* EncounterPointer = Encounter.Get();

	if (EncounterPointer)
	{
		EncounterPointer->RemoveCharacterFromEncounter(ActionComponent);
	}
}

void UTurn::NetMulticast_OnTurnStart_Implementation()
{
	if (Turn_DebugAll || Turn_DebugNetwork)
	{
		UE_LOG(LogRPG, Log, TEXT("Calling multicast start turn..."));
	}
	OnTurnStart.Broadcast(this);
	
}

void UTurn::NetMulticast_OnTurnEnd_Implementation()
{
	if (Turn_DebugAll || Turn_DebugNetwork)
	{
		UE_LOG(LogRPG, Log, TEXT("Calling multicast end turn..."));
	}
	OnTurnEnd.Broadcast(this);
}

void UTurn::OnRep_ActionComponent()
{
	ActionComponent->SetTurn(this);
}

void UTurn::OnRep_ActionPoints()
{
	OnActionPointsChange.Broadcast(this);
}

void UTurn::OnRep_Movement()
{
	OnMovementChanged.Broadcast(this);
}

void UTurn::OnRep_UsedActionPoints()
{
	OnActionPointsChange.Broadcast(this);
}

void UTurn::OnRep_UsedMovement()
{
	OnMovementChanged.Broadcast(this);
}

void UTurn::SetActionPoints(int32 val)
{
	 ActionPoints = val; 
}

void UTurn::SetMovement(float val)
{
	Movement = val;
}

void UTurn::SetUsedActionPoints(int32 val)
{
	UsedActionPoints = val;
}

void UTurn::SetUsedMovement(float val)
{
	UsedMovement = val;
}

void UTurn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTurn, UsedActionPoints);
	DOREPLIFETIME(UTurn, UsedMovement);
	DOREPLIFETIME(UTurn, ActionPoints);
	DOREPLIFETIME(UTurn, Movement);
	DOREPLIFETIME(UTurn, ActionComponent);
	DOREPLIFETIME(UTurn, Encounter);
	
}
