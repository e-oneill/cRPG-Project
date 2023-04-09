// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Triggerable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UTriggerable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPG_API ITriggerable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanTrigger(AActor* TriggeredBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryTrigger(AActor* TriggeredBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Trigger(AActor* TriggeredBy);
};
