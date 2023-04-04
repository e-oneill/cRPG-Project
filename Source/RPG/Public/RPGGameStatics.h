// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGGameStatics.generated.h"

class UEncounterManager;
class UGameplayActionComponent;
class ARPGPlayerController;

/**
 * 
 */
UCLASS()
class RPG_API URPGGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "RPG|Game State")
	static UEncounterManager* GetEncounterManager(UWorld* World);

	UFUNCTION(BlueprintPure, Category = "RPG|Game State")
	static ARPGPlayerController* GetActionComponentPlayerController(UGameplayActionComponent* ActionComponent);
};
