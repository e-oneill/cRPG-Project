// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RPGAIController.h"
#include "Perception/PawnSensingComponent.h"

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

void ARPGAIController::AddOrUpdateSpottedCharacter(UGameplayActionComponent* SpottedActionComponent)
{
	for (FSpottedCharacter& SpottedCharacter : SpottedCharacters)
	{
		if (SpottedCharacter.SpottedActionComponent == SpottedActionComponent)
		{
			SpottedCharacter.UpdateSpottedCharacter();
			return;
		}
	}

	FSpottedCharacter NewSpottedCharacter;
	NewSpottedCharacter.SpottedActionComponent = SpottedActionComponent;
	NewSpottedCharacter.UpdateSpottedCharacter();
	SpottedCharacters.Add(NewSpottedCharacter);
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
}

void ARPGAIController::HandlePawnSpotted(APawn* SeenPawn)
{
	UGameplayActionComponent* PawnActionComponent = Cast<UGameplayActionComponent>(SeenPawn->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (PawnActionComponent)
	{
		AddOrUpdateSpottedCharacter(PawnActionComponent);
	}
}

void ARPGAIController::HandleNoiseHeard(APawn* InInstigator, const FVector& Location, float Volume)
{

}
