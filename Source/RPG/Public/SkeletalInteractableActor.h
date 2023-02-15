// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "SkeletalInteractableActor.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class RPG_API ASkeletalInteractableActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	ASkeletalInteractableActor();

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* OnInteractMontage;

public:
	bool Interact_Implementation(AActor* Interactor) override;

	UAnimMontage* GetOnInteractMontage() const { return OnInteractMontage; }
	void SetOnInteractMontage(UAnimMontage* val) { OnInteractMontage = val; }
	USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }
	void SetMeshComponent(USkeletalMeshComponent* val) { MeshComponent = val; }
};
