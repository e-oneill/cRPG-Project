// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticInteractableActor.h"
#include "../Triggerable.h"
#include "SIA_SecretDoor.generated.h"

class UArrowComponent;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class RPG_API ASIA_SecretDoor : public AStaticInteractableActor, public ITriggerable
{
	GENERATED_BODY()

public:
	ASIA_SecretDoor();

	void Tick(float DeltaTime) override;


	bool CanTrigger_Implementation(AActor* TriggeredBy) override;


	void Trigger_Implementation(AActor* TriggeredBy) override;


	bool CanInteract_Implementation(AActor* Interactor) override;


	bool Interact_Implementation(AActor* Interactor) override;


	bool TryTrigger_Implementation(AActor* TriggeredBy) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* MoveDestination;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TopBoxCollider;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BottomBoxCollider;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	float MovementSpeed = 200;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	bool bCanRetrigger = true;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	bool bTriggerOnInteract = false;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	bool bIsTriggered;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	bool bBlockWhenOnTop;

	UPROPERTY(EditAnywhere, Category = "Triggerable Static Mover")
	bool bBlockWhenBelow;

	FVector StartLocation;
	FVector EndLocation;
	FRotator StartRotation;
	FRotator EndRotation;

	void BeginPlay() override;

	bool bCheckShouldMove(FVector LocationToMoveTo);

};
