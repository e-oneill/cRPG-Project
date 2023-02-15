// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_TargetHasActionComp.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

bool UAC_TargetHasActionComp::CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/, UAction* Action /*= nullptr*/)
{
	if (!TargetActor)
	{
		return false;
	}

	return TargetActor->GetComponentByClass(UGameplayActionComponent::StaticClass()) != nullptr;
}

void UAC_TargetHasActionComp::PostInitProperties()
{
	Super::PostInitProperties();
	ConsiderationAppliesTo = EConsiderationApplication::Execute;
}
