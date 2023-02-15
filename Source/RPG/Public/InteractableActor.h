// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PlayerInteraction/Interactable.h"
#include "InteractableActor.generated.h"

UCLASS()
class RPG_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InteractionDistance = 300.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanInteract_Implementation(AActor* Interactor) override;

	bool Interact_Implementation(AActor* Interactor) override;

};
