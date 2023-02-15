// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionConsideration.h"

bool UActionConsideration::CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/, UAction* Action /*= nullptr*/)
{
	return true;
}
