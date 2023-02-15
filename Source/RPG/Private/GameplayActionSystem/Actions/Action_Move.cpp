// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/Actions/Action_Move.h"
#include "ActionSystemTags.h"
#include "GameplayActionSystem/Considerations/AC_HasMovementPointsRemaining.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/GameplayActionTask.h"
#include "GameplayActionSystem/Tasks/GameplayTask_WaitForMove.h"
#include "AI/RPGAIController.h"

void UAction_Move::Tick_Implementation()
{
	if (State == EActionState::Preparing)
	{
		//provide the user with a prediction of the move path and movement cost
		//UE_LOG(LogRPG, Log, TEXT("Ticking Move Action..."));
	}

	if (State == EActionState::Executing)
	{
		//the move action is executing, so check if we should stop and if so, complete the action.
		
	}
}

void UAction_Move::OnMoveComplete(FMoveTaskResultData MoveTaskResult)
{
	CompleteAction();
}

void UAction_Move::SetupActionData()
{
	FGameplayTag Action_Move = FActionSystemTags::Get().Action_Move;
	if (!GrantsTags.HasTag(Action_Move))
	{
		GrantsTags.AddTag(Action_Move);
	}


	FGameplayTag Status_Stunned = FActionSystemTags::Get().Status_Stunned;
	if (!BlockedTags.HasTag(Status_Stunned))
	{
		BlockedTags.AddTag(Status_Stunned);
	}
	FGameplayTag Action_Attack = FActionSystemTags::Get().Action_Attack;
	if (!BlockedTags.HasTag(Action_Attack))
	{
		BlockedTags.AddTag(Action_Attack);
	}

	if (!ConsiderationTypes.Contains(UAC_HasMovementPointsRemaining::StaticClass()))
	{
		ConsiderationTypes.Add(UAC_HasMovementPointsRemaining::StaticClass());
	}
	bNoAutoStop = true;
	bFaceTarget = true;
}

void UAction_Move::ExecuteAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	if (Task.IsValid())
	{
		UGameplayActionTask* OldTask = Task.Get();
		OldTask->FinishTask();
	}
	Super::ExecuteAction_Implementation(TargetLocation, TargetActor);
	//first check that the data we need has been passed into the execution
	if (TargetLocation == FVector::ZeroVector && TargetActor == nullptr)
	{
		UE_LOG(LogRPG, Error, TEXT("Move action on %s was executed without either the target data fed into the execution function"), *Source->GetName());
		return;
	}
	MoveTargetActor = TargetActor;
	MoveTargetLocation = TargetLocation;

	//tell the AI Controller to move this character
	

	UGameplayTasksComponent* TasksComponent = Cast<UGameplayTasksComponent>(Source);
	if (TasksComponent)
	{
		Task = UGameplayActionTask::CreateGameplayActionTask(UGameplayTask_WaitForMove::StaticClass(), Source, this, TargetLocation, TargetActor);
		FGameplayResourceSet Resources;
		IGameplayTaskOwnerInterface& Interface = *Cast<IGameplayTaskOwnerInterface>(TasksComponent);
		Cast<UGameplayTask_WaitForMove>(Task)->OnMoveComplete.AddUniqueDynamic(this, &UAction_Move::OnMoveComplete);
		TasksComponent->RunGameplayTask(Interface, *Task, 0, Resources, Resources);
		
	}
	//instruct the action component to move the actor.
	//Do I need a GameplayTask class for this?

}

void UAction_Move::InitializeAction_Implementation(UGameplayActionComponent* InSource)
{
		Super::InitializeAction_Implementation(InSource);
}


