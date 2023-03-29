// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AI/WorldModel.h"
#include "PlanEntry.h"
#include "GOAPPlanningComponent.generated.h"


class AAIController;
class ARPGAIController;
class UGOAP_Goal;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UGOAPPlanningComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGOAPPlanningComponent();

protected:
	bool bIsPlanning;

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TArray<FWorldModel> WorldModels;

	UPROPERTY(EditDefaultsOnly)
	int32 PlanLength = 4;

	TArray<UAction*> AgentActions;

	TArray<FPlanEntry> CurrentPlan;

	/*UPROPERTY(VisibleInstanceOnly)
	FWorldModel InitialWorldModel;*/

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGOAP_Goal>> GoalClasses;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<UGOAP_Goal*> Goals;

	float ScoreWorldModel(FWorldModel& ModelToScore);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FPlanEntry> FindPlan();

	TArray<FPlanEntry> DFSForActionSequence();

	AAIController* GetAIController();

	ARPGAIController* GetRPGAIController();
};
