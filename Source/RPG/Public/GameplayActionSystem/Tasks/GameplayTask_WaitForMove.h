// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GameplayActionTask.h"
#include "../GameplayActionSystemStatics.h"
#include "GameplayTask_WaitForMove.generated.h"

class UAction_Move;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveTaskResultSignature, FMoveTaskResultData, MoveResult);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPG_API UGameplayTask_WaitForMove : public UGameplayActionTask
{
	GENERATED_BODY()
	
protected:
	void Activate() override;

	//UAction_Move* MoveAction;

	FVector LastLocationOfOwner;

	FMoveTaskResultData MoveResult;

	void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

public:
	void TickTask(float DeltaTime) override;

	void FinishTask_Implementation() override;

	UPROPERTY(BlueprintAssignable)
	FMoveTaskResultSignature OnMoveComplete;

};
