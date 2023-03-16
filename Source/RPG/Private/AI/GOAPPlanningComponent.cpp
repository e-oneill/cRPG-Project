// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GOAPPlanningComponent.h"
#include "AI/WorldModel.h"
#include "AI/RPGAIController.h"
#include "GameFramework/RPGGameState.h"

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
	
}


// Called every frame
void UGOAPPlanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetRPGAIController() && GetRPGAIController()->GetPlayerControlComponent() && !GetRPGAIController()->GetPlayerControlComponent()->IsPlayerControlled())
	{
		CreateActionPlan();
	}

	//CreateActionPlan();
	// ...
}

void UGOAPPlanningComponent::CreateActionPlan()
{
	FWorldModel InitialWorldModel;
	TArray<UAction*> Actions = GetRPGAIController()->GetActions();
	InitialWorldModel.CreateWorldModel(GetAIController());

	ARPGGameState* GameState = GetOwner()->GetWorld()->GetGameState<ARPGGameState>();

	WorldModels.Add(InitialWorldModel);

	FWorldModel FirstCheck = InitialWorldModel;

	for (FWorldModelActor& KnownActor : FirstCheck.KnownActors)
	{
		for (UAction* Action : Actions)
		{
			FirstCheck.Self.UseActionOnTargetActor(KnownActor, Action);
		}
	}

	WorldModels.Add(FirstCheck);
	
	

}

AAIController* UGOAPPlanningComponent::GetAIController()
{
	return Cast<AAIController>(GetOwner());
}

ARPGAIController* UGOAPPlanningComponent::GetRPGAIController()
{
	return Cast<ARPGAIController>(GetOwner());
}

