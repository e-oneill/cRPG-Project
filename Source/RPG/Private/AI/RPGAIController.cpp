// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RPGAIController.h"
#include "Perception/PawnSensingComponent.h"
#include "ActionSystemTags.h"
#include "../RPG.h"
#include "GameFramework/RPGGameState.h"
#include "GameState/EncounterManager.h"
#include "GameState/Encounter.h"
#include "AI/GOAPPlanningComponent.h"
#include "AI/WorldModel.h"
#include "RPGGameStatics.h"

ARPGAIController::ARPGAIController()
{
	GOAPPlanner = CreateDefaultSubobject<UGOAPPlanningComponent>(TEXT("GOAPPlanner"));
}

void ARPGAIController::AIMoveToLocation(FVector Location)
{
	MoveToLocation(Location);
}

bool ARPGAIController::HasSpottedCharacter(UGameplayActionComponent* SpottedActionComponent)
{
	for (FSpottedCharacter& SpottedCharacter : SpottedCharacters)
	{
		if (SpottedCharacter.SpottedActionComponent == SpottedActionComponent)
		{
			return true;
		}
	}

	return false;
}

FSpottedCharacter& ARPGAIController::AddOrUpdateSpottedCharacter(UGameplayActionComponent* SpottedActionComponent)
{
	for (FSpottedCharacter& SpottedCharacter : SpottedCharacters)
	{
		if (SpottedCharacter.SpottedActionComponent == SpottedActionComponent)
		{
			SpottedCharacter.UpdateSpottedCharacter();
			return SpottedCharacter;
		}
	}

	FSpottedCharacter NewSpottedCharacter;
	NewSpottedCharacter.SpottedActionComponent = SpottedActionComponent;
	NewSpottedCharacter.UpdateSpottedCharacter();
	SpottedCharacters.Add(NewSpottedCharacter);

	return SpottedCharacters[SpottedCharacters.Num() - 1];
}

TArray<UAction*> ARPGAIController::GetActions()
{
	TArray<UAction*> Actions;

	if (!ActionComponent)
	{
		return Actions;
	}

	return ActionComponent->GetCharacterActions();
}

void ARPGAIController::GetPlanAndExecute()
{
	ActionPlan = GOAPPlanner->FindPlan();

	ActionPlan.RemoveAt(0);
}

void ARPGAIController::ExecuteNextAction()
{
	if (ActionPlan.Num() == 0)
	{
		//Plan is empty
		HandleFinishPlan();
		return;
	}

	//if the next entry in plan is NULL, that means the DO_NOTHING action has been queued.
	while (ActionPlan[0].Action == NULL)
	{
		ActionPlan.RemoveAt(0);
		//if after removing the DO_NOTHING action the plan is empty, return
		if (ActionPlan.Num() == 0)
		{
			HandleFinishPlan();
			return;
		}
	}

	UAction* ActionTaken = ActionPlan[0].Action;
	

	bool bCanTakeAction = ExecutePlanEntry(ActionPlan[0]);
	//Plan.RemoveAt(0);

	if (!bCanTakeAction)
	{
		OnActionComplete(ActionTaken, EActionState::Complete, EActionState::Idle);
	}
	
	
}

bool ARPGAIController::ExecutePlanEntry(FPlanEntry& PlanEntry)
{
	FVector TargetLocation = FVector::ZeroVector;
	AActor* TargetActor = nullptr;
	UAction* Action = PlanEntry.Action;
	
	if (!PlanEntry.TargetActor || !PlanEntry.TargetActor->IsValidLowLevel())
	{
		TargetLocation = PlanEntry.TargetLocation;
	}
	else
	{
		TargetActor = PlanEntry.TargetActor->GetOwner();
	}

	//assume that this entry is item 0 - as there is no reliable way to == to match this specific entry
	ActionPlan.RemoveAt(0);

	bool bTakeAction = Action->TryExecuteAction(TargetLocation, TargetActor);

	

	if (bTakeAction)
	{
		Action->OnActionComplete.AddUniqueDynamic(this, &ARPGAIController::OnActionComplete);
	}

	return bTakeAction;
}

void ARPGAIController::HandleFinishPlan()
{
	ActionPlan = GOAPPlanner->FindPlan();
	if (ActionPlan.IsEmpty())
	{
		EndTurn();
	}
	else
	{
		ExecuteNextAction();
	}
}

void ARPGAIController::EndTurn()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (!TimerManager.IsTimerActive(EndTurnTimer))
	{
		TimerManager.SetTimer(EndTurnTimer, this, &ARPGAIController::EndTurnCallback, 0.5f, false, -1.f);
	}
	

	
}

void ARPGAIController::EndTurnCallback()
{
	if (Turn)
	{
		AEncounter* Encounter = Turn->GetEncounter().Get();
		if (Encounter->GetCurrentTurn() == Turn)
		{
			Encounter->EndCurrentTurn();
		}
	}
}

void ARPGAIController::OnTurnAssigned(UTurn* NewTurn)
{
	Turn = NewTurn;
	Turn->OnTurnStart.AddUniqueDynamic(this, &ARPGAIController::OnTurnStart);
}

void ARPGAIController::OnTurnStart(UTurn* NewTurn)
{
	if (PlayerControlComponent->IsPlayerControlled())
	{
		return;
	}

	ActionPlan = GOAPPlanner->FindPlan();
	if (ActionPlan.Num() > 0)
	{
		ExecuteNextAction();
	}
	else
	{
		EndTurn();
	}
}

void ARPGAIController::OnActionComplete(UAction* CompletedAction, EActionState State, EActionState OldState)
{
	if (State != EActionState::Complete)
	{
		return;
	}

	if (ActionPlan.Num() > 0)
	{
		ExecuteNextAction();
	}
	else
	{
		HandleFinishPlan();
	}
}

void ARPGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//throw std::logic_error("The method or operation is not implemented.");

	UPawnSensingComponent* PawnSensingComponent = Cast<UPawnSensingComponent>(InPawn->GetComponentByClass(UPawnSensingComponent::StaticClass()));
	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddUniqueDynamic(this, &ARPGAIController::HandlePawnSpotted);
		PawnSensingComponent->OnHearNoise.AddUniqueDynamic(this, &ARPGAIController::HandleNoiseHeard);
	}

	PlayerControlComponent = Cast<UPlayerControlComponent>(InPawn->GetComponentByClass(UPlayerControlComponent::StaticClass()));
	ActionComponent = Cast<UGameplayActionComponent>(InPawn->GetComponentByClass(UGameplayActionComponent::StaticClass()));

	if (ActionComponent)
	{
		ActionComponent->OnTurnAssigned.AddUniqueDynamic(this, &ARPGAIController::OnTurnAssigned);
	}

}

void ARPGAIController::HandlePawnSpotted(APawn* SeenPawn)
{
	UGameplayActionComponent* PawnActionComponent = Cast<UGameplayActionComponent>(SeenPawn->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (PawnActionComponent)
	{
		FSpottedCharacter& SpottedCharacter = AddOrUpdateSpottedCharacter(PawnActionComponent);
		FGameplayTag PlayerFaction = FActionSystemTags::Get().Player_Faction;
		ARPGGameState* GameState = SeenPawn->GetWorld()->GetGameState<ARPGGameState>();
		//if this AI controller is not a member of the player faction and is hostile to the player faction, start a combat
		if (GameState && 
			(ActionComponent->GetFaction() != PlayerFaction && GameState->IsFactionHostile(ActionComponent->GetFaction(), PawnActionComponent->GetFaction())) && 
			SpottedCharacter.GetTargetAwareness() < 1.f)
		{
			SpottedCharacter.SetTargetAwareness(SpottedCharacter.GetTargetAwareness() + AlertnessTickRate);
			Alertness += AlertnessTickRate;
			UE_LOG(LogRPG, Log, TEXT("Spotted a hostile player character, awareness of this target is now %f!"), SpottedCharacter.GetTargetAwareness());
			if (SpottedCharacter.GetTargetAwareness() >= 1.f)
			{
				//declaring bools for handling the possible cases here
				bool bSpottedCharInEncounter = SpottedCharacter.SpottedActionComponent->GetTurn() != nullptr;
				bool bAIAgentInEncounter = ActionComponent->GetTurn() != nullptr;
				

				//if the spotted ca
				if (bSpottedCharInEncounter && !bAIAgentInEncounter)
				{
					AEncounter* SpottedCharacterEncounter = SpottedCharacter.SpottedActionComponent->GetTurn()->GetEncounter().Get();
					SpottedCharacterEncounter->AddCharacterToEncounter(ActionComponent);
				}
				else if (bSpottedCharInEncounter && bAIAgentInEncounter && SpottedCharacter.SpottedActionComponent->GetTurn()->GetEncounter().Get() != ActionComponent->GetTurn()->GetEncounter().Get())
				{

					CombineEncountersWithSpottedCharacter(SpottedCharacter.SpottedActionComponent);
				}
				else if (!bSpottedCharInEncounter && bAIAgentInEncounter)
				{
					AddSpottedCharacterToMyEncounter(SpottedCharacter.SpottedActionComponent);
				}
				else if (!bSpottedCharInEncounter && !bAIAgentInEncounter)
				{
					StartEncounterWithSpottedCharacter(PawnActionComponent);
				}
			}
		}
	}
}

void ARPGAIController::HandleNoiseHeard(APawn* InInstigator, const FVector& Location, float Volume)
{

}

void ARPGAIController::StartEncounterWithSpottedCharacter(UGameplayActionComponent* SpottedCharacter)
{
	UEncounterManager* EncounterManager = URPGGameStatics::GetEncounterManager(SpottedCharacter->GetWorld());
	if (!EncounterManager)
	{
		return;
	}

	TArray<UGameplayActionComponent*> EncounterCharacters;
	EncounterCharacters.Add(ActionComponent);
	EncounterCharacters.Add(SpottedCharacter);

	EncounterManager->StartEncounter(EncounterCharacters);

}

void ARPGAIController::AddSpottedCharacterToMyEncounter(UGameplayActionComponent* SpottedCharacter)
{
	
	AEncounter* MyEncounter = ActionComponent->GetTurn()->GetEncounter().Get();
	AddComponentToEncounter(SpottedCharacter, MyEncounter);
}

void ARPGAIController::JoinEncounterWithSpottedCharacter(UGameplayActionComponent* SpottedCharacter)
{
	AEncounter* TheirEncounter = SpottedCharacter->GetTurn()->GetEncounter().Get();
	AddComponentToEncounter(ActionComponent, TheirEncounter);
}

void ARPGAIController::AddComponentToEncounter(UGameplayActionComponent* ComponentToAdd, AEncounter* EncounterToJoin)
{
	UEncounterManager* EncounterManager = URPGGameStatics::GetEncounterManager(ComponentToAdd->GetWorld());
	if (!EncounterManager)
	{
		return;
	}
	if (!EncounterToJoin)
	{
		UE_LOG(LogRPG, Error, TEXT("This character has a turn without an encounter"));
		return;
	}
	EncounterManager->AddComponentToEncounter(ComponentToAdd, EncounterToJoin);
}

void ARPGAIController::CombineEncountersWithSpottedCharacter(UGameplayActionComponent* SpottedCharacter)
{
	UEncounterManager* EncounterManager = URPGGameStatics::GetEncounterManager(SpottedCharacter->GetWorld());
	if (!EncounterManager)
	{
		return;
	}
	AEncounter* SpottedCharacterEncounter = SpottedCharacter->GetTurn()->GetEncounter().Get();
	AEncounter* MyEncounter = ActionComponent->GetTurn()->GetEncounter().Get();
	EncounterManager->CombineEncounters(MyEncounter, SpottedCharacterEncounter);
}



