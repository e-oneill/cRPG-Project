// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GOAPPlanningComponent.h"
#include "AI/WorldModel.h"
#include "AI/RPGAIController.h"
#include "GameFramework/RPGGameState.h"
#include "AI/GOAP_Goal.h"
#include "../RPG.h"

// Sets default values for this component's properties
UGOAPPlanningComponent::UGOAPPlanningComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGOAPPlanningComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	for (TSubclassOf<UGOAP_Goal> GoalClass : GoalClasses)
	{
		Goals.Add(UGOAP_Goal::CreateGOAPGoal(GoalClass, this));
	}

}


float UGOAPPlanningComponent::ScoreWorldModel(FWorldModel& ModelToScore)
{
	float ModelDiscontent = 0;

	for (UGOAP_Goal* Goal : Goals)
	{
		float Insistence = Goal->ScoreInsistence(ModelToScore);
		float Discontent = Goal->GetDiscontent(Insistence);
		ModelDiscontent += Discontent;
	}

	return ModelDiscontent;
}

// Called every frame
void UGOAPPlanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/*if (GetRPGAIController() && GetRPGAIController()->GetPlayerControlComponent() && !GetRPGAIController()->GetPlayerControlComponent()->IsPlayerControlled())
	{
		CurrentPlan = FindPlan();
	}*/

}

TArray<FPlanEntry> UGOAPPlanningComponent::FindPlan()
{
	//setup and ensure this won't start planning while a plan being sought
	if (bIsPlanning)
	{
		return TArray<FPlanEntry>();
	}
	bIsPlanning = true;

	TArray<FPlanEntry> ActionPlan = DFSForActionSequence();

	bIsPlanning = false;

	return ActionPlan;
}

TArray<FPlanEntry> UGOAPPlanningComponent::DFSForActionSequence()
{
	//Get the actions that this agent can take
	AgentActions = GetRPGAIController()->GetActions();
	float BaseDiscontent = 0;
	//Store the initial state of the world
	FWorldModel InitialWorldModel;
	InitialWorldModel.CreateWorldModel(GetAIController());
	InitialWorldModel.CurrentActionIndex = 0;
	BaseDiscontent = ScoreWorldModel(InitialWorldModel);

	//UE_LOG(LogRPG, Log, TEXT("Discontent of initial world model is: %f"), BaseDiscontent);

	TArray<FWorldModel> PlanWorldModels;
	TArray<UAction*> BestActionSequence;
	TArray<FPlanEntry> ActionPlan;
	TArray<FPlanEntry> BestActionPlan;
	BestActionSequence.Init(nullptr, PlanLength);
	ActionPlan.Init(FPlanEntry(), PlanLength);
	PlanWorldModels.Add(InitialWorldModel);
	
	int CurrentDepth = 0;
	int CurrentActionIndex = 0;
	int BestDiscontent = BaseDiscontent;
	FWorldModel CurrentWorldModel;
	TArray<UAction*> CurrentActionSequence;

	while (CurrentDepth >= 0)
	{
		if (CurrentDepth >= PlanLength)
		{
			float CurrentDiscontent = ScoreWorldModel(PlanWorldModels[CurrentDepth]);
			if (CurrentDiscontent < BestDiscontent)
			{
				BestDiscontent = CurrentDiscontent;
				BestActionPlan = ActionPlan;
			}
			CurrentDepth--;
			
		}
		else
		{
			if (PlanWorldModels[CurrentDepth].CurrentActionIndex >= AgentActions.Num())
			{
				CurrentDepth--;
				
				continue;
			}
			UAction* CurrentAction = AgentActions[PlanWorldModels[CurrentDepth].CurrentActionIndex++];

			//CurrentActionIndex++;
			FWorldModel NewModel = PlanWorldModels[CurrentDepth];
			NewModel.CurrentActionIndex = 0;
			FWorldModelActor& Target = NewModel.GetHighestPriorityTarget();
			if (NewModel.Self.CheckCanExecuteAction(CurrentAction, Target))
			{
				FPlanEntry NextPlanEntry = 
				NewModel.Self.UseActionOnTargetActor(Target, CurrentAction);
				BestActionSequence[CurrentDepth] = CurrentAction;
				ActionPlan[CurrentDepth] = NextPlanEntry;
			}


			if (PlanWorldModels.Num() > CurrentDepth + 1)
				PlanWorldModels[CurrentDepth + 1] = NewModel;
			else
				PlanWorldModels.Add(NewModel);

			CurrentDepth++;
		}
	}

	return BestActionPlan;
}

AAIController* UGOAPPlanningComponent::GetAIController()
{
	return Cast<AAIController>(GetOwner());
}

ARPGAIController* UGOAPPlanningComponent::GetRPGAIController()
{
	return Cast<ARPGAIController>(GetOwner());
}

