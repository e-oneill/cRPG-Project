// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/Tasks/GameplayTask_WaitForMove.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Actions/Action_Move.h"
#include "GameState/Turn.h"
#include "AI/RPGAIController.h"
#include "DrawDebugHelpers.h"

extern TAutoConsoleVariable<bool> CVarDebugAll;
bool WaitMove_DebugAll = CVarDebugAll.GetValueOnGameThread();

extern TAutoConsoleVariable<bool> CVarDebugActionSystem;
bool WaitMove_DebugActionSystem = CVarDebugActionSystem.GetValueOnGameThread();

void UGameplayTask_WaitForMove::Activate()
{
	if (!OwningActionComponent)
	{
		UE_LOG(LogRPG, Error, TEXT("A move task tried to fire, but the Tasks Component is not a Gameplay Action Component"));
		EndTask();
		return;
	}
	//Set the MoveResult to the interuppted default, so if the task doesn't end itself it stays as this
	MoveResult.MoveResult = EMoveTaskResult::Stopped;
	//store a pointer to the action that started this, cast to the move subclass
	//MoveAction = Cast<UAction_Move>(OwningAction);
	
	ARPGAIController* AIController = OwningActionComponent->GetAIController();
	if (AIController)
	{
		AIController->AIMoveToLocation(TargetLocation);
	}

	bTickingTask = true;
	LastLocationOfOwner = OwningActionComponent->GetOwner()->GetActorLocation();
	Super::Activate();
}

void UGameplayTask_WaitForMove::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
	
}

void UGameplayTask_WaitForMove::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	/*if (!MoveAction)
	{
		//if for whatever reason move action is invalid, task should end.
		EndTask();
		return;
	}*/
	
	//get the current location of owner
	
	FVector LocationOfOwner = OwningActionComponent->GetOwner()->GetActorLocation();
	LocationOfOwner.Z -= 80.f;
	FVector MoveTarget;
	//get the move stop location. If the move target is another actor, this will not be a nullptr.
	if (TargetActor)
	{
		MoveTarget = TargetActor->GetActorLocation();
	}
	//if move target actor is a null pointer, we must be moving to a location;
	else
	{
		MoveTarget = TargetLocation;
	}
	
	//if the action component has a turn, modify the turn's movement remaining
	if (OwningActionComponent->GetTurn())
	{
		float CurrentUsedMovement = OwningActionComponent->GetTurn()->GetUsedMovement();
		float DistanceMovedSinceLastTick = FVector::Dist(LocationOfOwner, LastLocationOfOwner);
		
		
		if (DistanceMovedSinceLastTick > 0)
		{
			UActionAttribute* MoveAttribute = OwningActionComponent->GetTurn()->GetMovementAttribute();
			//OwningActionComponent->GetTurn()->SetUsedMovement(CurrentUsedMovement + DistanceMovedSinceLastTick);
			if (MoveAttribute)
			{
				MoveAttribute->SetAttributeValue(MoveAttribute->GetAttributeValue() - DistanceMovedSinceLastTick);
			}
		}
	}

	//now that we have the start location and end location, we can decide whether this task should be ending.
	//reasons the action could end: 
		//A: we are within our desired range of the target actor
		//B: if we are in turn-based mode, if we have ran out of movement for this turn.
	//start by setting the distance we will use to check for our desired range. If the target is another actor, we want to move to within range of them. If the target is just a location, we want to move straight there
	float DistanceToLookFor;
	if (TargetActor)
	{
		DistanceToLookFor = 200.f;
	}
	else
	{
		DistanceToLookFor = 100.f;
	}
	if (WaitMove_DebugAll || WaitMove_DebugActionSystem)
	{
		DrawDebugSphere(GetOwnerActor()->GetWorld(), LocationOfOwner, 100.f, 32, FColor::Black);
		DrawDebugSphere(GetOwnerActor()->GetWorld(), MoveTarget, 100.f, 32, FColor::White);
	}
	
	if (TimeRunning > 1.f && FVector::Dist(LastLocationOfOwner, LocationOfOwner) <= 0.f )
	{
		MoveResult.Success = false;
		FinishTask();
		if (WaitMove_DebugAll || WaitMove_DebugActionSystem)
			UE_LOG(LogRPG, Log, TEXT("Ending move action because we stood still for too long."));
		return;
	}

	if (FVector::Dist(LocationOfOwner, MoveTarget) <= DistanceToLookFor)
	{
		MoveResult.Success = true;
		MoveResult.MoveResult = EMoveTaskResult::ReachedTarget;
		if (WaitMove_DebugAll || WaitMove_DebugActionSystem)
			UE_LOG(LogRPG, Log, TEXT("Ending move action, we are near location."));
		FinishTask();
		return;
	}
	if (OwningActionComponent->GetTurn() && OwningActionComponent->GetTurn()->GetRemainingMovement() <= 0.f)
	{
		MoveResult.Success = false;
		MoveResult.MoveResult = EMoveTaskResult::RanOutOfMovement;
		FinishTask();	
		if (WaitMove_DebugAll || WaitMove_DebugActionSystem)
			UE_LOG(LogRPG, Log, TEXT("Ending move action, we are out of movement."));
		return;
	}
	LastLocationOfOwner = LocationOfOwner;
	//UE_LOG(LogRPG, Log, TEXT("Ticking movement task."));

}

void UGameplayTask_WaitForMove::FinishTask_Implementation()
{
	ARPGAIController* AIController = OwningActionComponent->GetAIController();
	if (AIController)
	{
		AIController->StopMovement();
	}
	OnMoveComplete.Broadcast(MoveResult);

	Super::FinishTask_Implementation();
}
