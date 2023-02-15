// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "StaticInteractableActor.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class RPG_API AStaticInteractableActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	AStaticInteractableActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
};
