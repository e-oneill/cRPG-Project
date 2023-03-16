// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RPGAIController.h"
#include "Perception/PawnSensingComponent.h"
#include "ActionSystemTags.h"
#include "../RPG.h"
#include "GameFramework/RPGGameState.h"
#include "GameState/EncounterManager.h"
#include "GameState/Encounter.h"
#include "AI/GOAPPlanningComponent.h"

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
				if (!ActionComponent->GetTurn())
				{
					//we are alerted, start a combat encounter
					StartEncounterWithSpottedCharacter(PawnActionComponent);
				}
				else
				{
					//we have a turn, so add the character to this encounter
					UE_LOG(LogTemp, Warning, TEXT("I am already in an encounter!"));

					AEncounter* MyEncounter = ActionComponent->GetTurn()->GetEncounter().Get();
					if (!MyEncounter)
					{
						UE_LOG(LogRPG, Error, TEXT("This character has a turn without an encounter"));
					}
					MyEncounter->AddCharacterToEncounter(SpottedCharacter.SpottedActionComponent);
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
	ARPGGameState* GameState = SpottedCharacter->GetWorld()->GetGameState<ARPGGameState>();
	if (!GameState)
	{
		return;
	}
	
	UEncounterManager* EncounterManager = GameState->GetEncounterManager();
	if (!EncounterManager)
	{
		return;
	}

	TArray<UGameplayActionComponent*> EncounterCharacters;
	EncounterCharacters.Add(ActionComponent);
	EncounterCharacters.Add(SpottedCharacter);

	EncounterManager->StartEncounter(EncounterCharacters);

}
