// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionCue_Animation.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionOwnerInterface.h"

void UActionCue_Animation::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation);

	AnimationMontage = ConfigData.AnimationMontage;
}

float UActionCue_Animation::GetCueLength()
{
	return AnimationMontage->GetPlayLength();
}

void UActionCue_Animation::PlayCue_Implementation()
{
	TScriptInterface<IActionOwnerInterface> ActionOwnerInterface = Target->GetActionOwner();
	if (ActionOwnerInterface.GetObject())
	{
		UAnimInstance* AnimInstance = IActionOwnerInterface::Execute_GetOwnerAnimInstance(ActionOwnerInterface.GetObject());
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AnimationMontage);
		}
	}
}
