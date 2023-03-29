#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "PlanEntry.generated.h"

USTRUCT(BlueprintType)
struct FPlanEntry
{
	GENERATED_BODY();

	UAction* Action;

	FVector TargetLocation;

	UGameplayActionComponent* TargetActor;
};