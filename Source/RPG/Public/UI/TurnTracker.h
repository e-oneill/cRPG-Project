// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnTracker.generated.h"

class UTurn;

/**
 * 
 */
UCLASS()
class RPG_API UTurnTracker : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Init(UTurn* InTurn);

protected:
	UTurn* Turn;

	bool bIsActive;

	UFUNCTION(BlueprintNativeEvent)
	void OnTurnStart(UTurn* InTurn);

	UFUNCTION(BlueprintNativeEvent)
	void OnTurnEnd(UTurn* InTurn);
};
