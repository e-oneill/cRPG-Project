// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AInteractableActor::CanInteract_Implementation(AActor* Interactor)
{
	return FVector::Dist(Interactor->GetActorLocation(), GetActorLocation()) <= InteractionDistance;
}

bool AInteractableActor::Interact_Implementation(AActor* Interactor)
{
	return IInteractable::Execute_CanInteract(this, Interactor);
}

