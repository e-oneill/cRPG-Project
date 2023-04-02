#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "PlanEntry.generated.h"

USTRUCT(BlueprintType)
struct FPlanEntry
{
	GENERATED_BODY();

	UPROPERTY()
	UAction* Action;

	FVector TargetLocation;

	UPROPERTY()
	UGameplayActionComponent* TargetActor;

	bool operator==(FPlanEntry const& otherEntry)
	{
		return Action == otherEntry.Action && TargetLocation == otherEntry.TargetLocation && TargetActor == otherEntry.TargetActor;
	}
};