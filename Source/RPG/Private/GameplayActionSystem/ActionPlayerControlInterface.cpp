// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionPlayerControlInterface.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "GameplayActionSystem/Action.h"

// Add default functionality here for any IActionPlayerControlInterface functions that are not pure virtual.

void IActionPlayerControlInterface::Client_PlayActionCue_Implementation(UAction* Action, const FCueConfigurationData& ConfigData, UGameplayActionComponent* TargetComponent, const FVector& TargetLocation)
{
	UActionCueBase::CreateAndPlayCue(ConfigData, Action->GetSource(), TargetComponent, TargetLocation);
}
