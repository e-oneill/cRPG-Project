// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_InActionRange.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

bool UAC_InActionRange::CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/, UAction* Action /*= nullptr*/)
{
	if (!Action)
	{
		return false;
	}
	FVector InternalLocation = TargetLocation;
	//World geometry StaticMeshActors may cause this to behave strangely, worth finding a way to check that the target actor is not world geometry
	if (TargetActor)
	{
		InternalLocation = TargetActor->GetActorLocation();
	}
	FVector OwnerLocation = Actor->GetOwner()->GetActorLocation();
	float DistToCheck = Action->GetActionRange();
	float Distance = FVector::Dist(OwnerLocation, InternalLocation);

	return Distance <= DistToCheck;
}

void UAC_InActionRange::PostInitProperties()
{
	Super::PostInitProperties();

	ConsiderationAppliesTo = EConsiderationApplication::Execute;
}
