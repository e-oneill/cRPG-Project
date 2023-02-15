// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/Action.h"
#include "Action_Move.generated.h"

class UGameplayActionTask;

/**
 * 
 */
UCLASS()
class RPG_API UAction_Move : public UAction
{
	GENERATED_BODY()
	
public:
	void InitializeAction_Implementation(UGameplayActionComponent* InSource) override;

	void ExecuteAction_Implementation(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr) override;

	void Tick_Implementation() override;

	

	#pragma region GettersSetters
	FVector GetMoveTargetLocation() const { return MoveTargetLocation; }
	void SetMoveTargetLocation(FVector val) { MoveTargetLocation = val; }
	AActor* GetMoveTargetActor() const { return MoveTargetActor; }
	void SetMoveTargetActor(AActor* val) { MoveTargetActor = val; }
	#pragma endregion GettersSetters

	//this UFUNCTION is a call back when the MoveTask that the action starts is complete. 
	UFUNCTION()
	virtual void OnMoveComplete(FMoveTaskResultData MoveTaskResult);

protected:
	TWeakObjectPtr<UGameplayActionTask> Task;

	void SetupActionData() override;

	FVector MoveTargetLocation;

	AActor* MoveTargetActor;

};
