// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WorldModel.h"
#include "GOAP_Goal.generated.h"

class UGOAPPlanningComponent;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPG_API UGOAP_Goal : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	ARPGGameState* GetGameState() const;

	static UGOAP_Goal* CreateGOAPGoal(TSubclassOf<UGOAP_Goal> TaskClass, UGOAPPlanningComponent* InPlanningComponent);

	UFUNCTION(BlueprintNativeEvent)
	float ScoreInsistence(FWorldModel ModelToScore);

	UFUNCTION(BlueprintNativeEvent)
	float GetDiscontent(float Insistence);

	UGOAPPlanningComponent* GetPlanningComponent() const { return PlanningComponent; }
	void SetPlanningComponent(UGOAPPlanningComponent* val) { PlanningComponent = val; }



protected:
	UPROPERTY(BlueprintReadOnly)
	UGOAPPlanningComponent* PlanningComponent;
	
};
