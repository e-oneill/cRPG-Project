// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "ActionConsideration.generated.h"

class UGameplayActionComponent;
class UAction;

UENUM(BlueprintType)
enum class EConsiderationApplication : uint8
{
	Both,
	Prepare,
	Execute
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class RPG_API UActionConsideration : public UReplicatedObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool CheckConsideration(UGameplayActionComponent* Actor, FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr, UAction* Action = nullptr);

	EConsiderationApplication GetConsiderationAppliesTo() const { return ConsiderationAppliesTo; }
	void SetConsiderationAppliesTo(EConsiderationApplication val) { ConsiderationAppliesTo = val; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConsiderationApplication ConsiderationAppliesTo;

};
