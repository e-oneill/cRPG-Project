// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/GameplayActionTask.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "../RPG.h"

UGameplayActionTask* UGameplayActionTask::CreateGameplayActionTask(TSubclassOf<UGameplayActionTask> TaskClass, UGameplayActionComponent* ActionComponent, UAction* Action, FVector TaskTargetLocation/* = FVector::ZeroVector*/, AActor* TaskTargetActor /*= nullptr*/)
{
	if (!Action || !ActionComponent)
	{
		UE_LOG(LogRPG, Warning, TEXT("%s was created without an action component or an action, check if this was intentional"), *TaskClass.GetDefaultObject()->GetName());
	}
	UGameplayActionTask* NewTask = NewObject<UGameplayActionTask>(ActionComponent->GetOwner(), TaskClass);
	NewTask->SetOwningAction(Action);
	NewTask->SetTargetLocation(TaskTargetLocation);
	NewTask->SetTargetActor(TaskTargetActor);
	NewTask->SetOwningActionComponent(ActionComponent);
	return NewTask;
}

void UGameplayActionTask::FinishTask_Implementation()
{
	EndTask();
}

void UGameplayActionTask::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	TimeRunning = GetWorld()->GetTimeSeconds() - TimeStart;
}

void UGameplayActionTask::Activate()
{
	Super::Activate();
	TimeStart = GetWorld()->GetTimeSeconds();
}
