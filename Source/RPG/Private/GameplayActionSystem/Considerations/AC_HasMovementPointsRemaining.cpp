// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/Considerations/AC_HasMovementPointsRemaining.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameState/Turn.h"

bool UAC_HasMovementPointsRemaining::CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/, UAction* Action)
{
	if (!Actor->GetTurn())
	{
		//later check for statuses that block movement
		return true;
	}
	
	UTurn* ActorTurn = Actor->GetTurn();
	if (ActorTurn->GetUsedMovement() < ActorTurn->GetMovement())
	{
		return true;
	}
	else
	{
		return false;
	}
}
