// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceVisible.generated.h"

class UTexture2D;

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UInterfaceVisible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPG_API IInterfaceVisible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual FName GetNameForUI()
	{
		return FName("Unnamed");
	}

	UFUNCTION(BlueprintCallable)
	virtual UTexture2D* GetImageForUI()
	{
		return nullptr;
	}

};
