// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "ActionCue_Animation.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UActionCue_Animation : public UActionCueBase
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimationMontage;

public:
	void InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction) override;

	void PlayCue_Implementation() override;

	float GetCueLength() override;

};
