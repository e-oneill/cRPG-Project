// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "../GameplayActionSystemStatics.h"
#include "ActionCue_Audio.generated.h"

class UAction;
/**
 * 
 */
UCLASS()
class RPG_API UActionCue_Audio : public UActionCueBase
{
	GENERATED_BODY()
	
public:
	void InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction) override;

	void PlayCue_Implementation() override;

	float GetCueLength() override;

	UFUNCTION()
	void StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState);

protected:
	UPROPERTY(BlueprintReadOnly)
	USoundCue* SoundCue;
	UPROPERTY(BlueprintReadOnly)
	UAudioComponent* PlayingSound;
	UPROPERTY(BlueprintReadOnly)
	bool bLooping = false;
	UPROPERTY(BlueprintReadOnly)
	bool b2DSound = false;
	ECueExecuteTime EndOn;
};
