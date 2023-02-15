// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GameplayActionSystemStatics.h"
#include "GameplayActionTask.generated.h"

class UGameplayActionComponent;
class UAction;

/**
 * 
 */
UCLASS()
class RPG_API UGameplayActionTask : public UGameplayTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UGameplayActionTask* CreateGameplayActionTask(TSubclassOf<UGameplayActionTask> TaskClass, UGameplayActionComponent* ActionComponent, UAction* Action, FVector TaskTargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	//At this base level, this method just calls EndTask. Later in the inheritance chain, you can use this to fire an event that an action is waiting for
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FinishTask();

	#pragma region GettersSetters
	UAction* GetOwningAction() const { return OwningAction; }
	void SetOwningAction(UAction* val) { OwningAction = val; }
	UGameplayActionComponent* GetOwningActionComponent() const { return OwningActionComponent; }
	void SetOwningActionComponent(UGameplayActionComponent* val) { OwningActionComponent = val; }
	FVector GetTargetLocation() const { return TargetLocation; }
	void SetTargetLocation(FVector val) { TargetLocation = val; }
	AActor* GetTargetActor() const { return TargetActor; }
	void SetTargetActor(AActor* val) { TargetActor = val; }
	#pragma endregion GettersSetters

	

	void TickTask(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Tasks")
	UAction* OwningAction;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Tasks")
	UGameplayActionComponent* OwningActionComponent;

	FTaskResultData TaskResults;

	FVector TargetLocation;

	AActor* TargetActor;

	float TimeStart;

	float TimeRunning;

	void Activate() override;

};
