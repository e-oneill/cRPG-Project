// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "GameplayActionSystemStatics.h"
#include "ActionCueBase.generated.h"

class UGameplayActionComponent;
class UAction;

/**
 *	THIS IS THE BASE CLASS FOR THE ACTION CUE, ONE OF THE MORE SPECIFIC CUE CLASSES SHOULD BE USED DEPENDING ON THE KIND OF ARTIFACT YOU WANT TO SPAWN
 */
UCLASS(Abstract, Blueprintable)
class RPG_API UActionCueBase : public UReplicatedObject
{
	GENERATED_BODY()

public:
	static UActionCueBase* CreateActionCue(FCueConfigurationData ConfigData, UAction* ParentAction, UGameplayActionComponent* InSource = nullptr, UGameplayActionComponent* InTarget = nullptr, FVector InTargetLocation = FVector::ZeroVector, TSubclassOf<UActionCueBase> ActionClass = UActionCueBase::StaticClass());
	static void CreateAndPlayCue(FCueConfigurationData ConfigData, UAction* ParentAction, UGameplayActionComponent* InSource = nullptr, UGameplayActionComponent* InTarget = nullptr, FVector InTargetLocation = FVector::ZeroVector, TSubclassOf<UActionCueBase> ActionClass = UActionCueBase::StaticClass());

	virtual void InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction);

	UFUNCTION()
	virtual void StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState);

	UFUNCTION(BlueprintNativeEvent)
	void PlayCue();

	//to make this cue tick, subscribe to tick events on the source action component using Source->OnTick.AddUniqueDynamic()
	UFUNCTION()
	virtual void TickCue(float DeltaTime);

	virtual float GetCueLength();

	#pragma region GettersSetters
	UGameplayActionComponent* GetSource() const { return Source; }
	void SetSource(UGameplayActionComponent* val) { Source = val; }
	UGameplayActionComponent* GetTarget() const { return Target; }
	void SetTarget(UGameplayActionComponent* val) { Target = val; }
	FVector GetTargetLocation() const { return TargetLocation; }
	void SetTargetLocation(FVector val) { TargetLocation = val; }
	ECueType GetType() const { return Type; }
	void SetType(ECueType val) { Type = val; }
	ECueTarget GetCueTarget() const { return CueTarget; }
	void SetCueTarget(ECueTarget val) { CueTarget = val; }
	ECueExecuteTime GetExecuteTime() const { return ExecuteTime; }
	void SetExecuteTime(ECueExecuteTime val) { ExecuteTime = val; }
	#pragma endregion GettersSetters

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Cue|Setup")
	UAction* Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	UGameplayActionComponent* Source;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	UGameplayActionComponent* Target;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	FVector TargetLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	ECueType Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	ECueTarget CueTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	ECueExecuteTime ExecuteTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action Cue|Setup")
	ECueActionStatusContext ExecutesOnActionStatus;
	UPROPERTY(BlueprintReadOnly)
		bool bLooping = false;
	UPROPERTY(BlueprintReadOnly)
	ECueExecuteTime EndOn;

};
