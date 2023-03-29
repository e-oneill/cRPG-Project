// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GOAP_Goal.h"
#include "AI/WorldModel.h"
#include "../RPG.h"
#include "AI/GOAPPlanningComponent.h"
#include "GameFramework/RPGGameState.h"

UWorld* UGOAP_Goal::GetWorld() const
{
	if (!PlanningComponent || !PlanningComponent->GetOwner())
	{
		return nullptr;
	}

	return PlanningComponent->GetOwner()->GetWorld();

}

ARPGGameState* UGOAP_Goal::GetGameState() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	return GetWorld()->GetGameState<ARPGGameState>();
}

UGOAP_Goal* UGOAP_Goal::CreateGOAPGoal(TSubclassOf<UGOAP_Goal> GoalClass, UGOAPPlanningComponent* InPlanningComponent)
{
	if (!InPlanningComponent)
	{
		UE_LOG(LogRPG, Warning, TEXT("%s was created without a planner"), *GoalClass.GetDefaultObject()->GetName());
	}
	UGOAP_Goal* NewGoal = NewObject<UGOAP_Goal>(InPlanningComponent->GetOwner(), GoalClass);
	NewGoal->SetPlanningComponent(InPlanningComponent);
	return NewGoal;
}

float UGOAP_Goal::ScoreInsistence_Implementation(FWorldModel ModelToScore)
{
	return 0.f;
}

float UGOAP_Goal::GetDiscontent_Implementation(float Insistence)
{
	return Insistence*Insistence;
}
