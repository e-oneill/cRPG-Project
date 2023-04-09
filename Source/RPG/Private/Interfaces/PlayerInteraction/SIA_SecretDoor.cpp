// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PlayerInteraction/SIA_SecretDoor.h"
#include "StaticInteractableActor.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "../RPGCharacter.h"

ASIA_SecretDoor::ASIA_SecretDoor() : AStaticInteractableActor()
{
	TopBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("TopBoxCollider"));
	TopBoxCollider->SetupAttachment(RootComponent);

	BottomBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomBoxCollider"));
	BottomBoxCollider->SetupAttachment(RootComponent);

	MoveDestination = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveComponent"));
	MoveDestination->SetupAttachment(RootComponent);

	this->SetActorTickEnabled(true);
}

void ASIA_SecretDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector MeshLocation = MeshComponent->GetComponentLocation();
	FRotator MeshRotation = MeshComponent->GetComponentRotation();
	if (bIsTriggered && (FVector::Dist(MeshLocation, EndLocation) > 1))
	{
		FVector NewTarget = FMath::VInterpConstantTo(MeshLocation, EndLocation, DeltaTime, MovementSpeed);
		FRotator NewTargetRotation = FMath::RInterpConstantTo(MeshRotation, EndRotation, DeltaTime, MovementSpeed);
		if (bCheckShouldMove(NewTarget))
		{
			MeshComponent->SetWorldLocationAndRotation(NewTarget, NewTargetRotation);
		}
	}
	else if (!bIsTriggered && bCanRetrigger && (FVector::Dist(MeshLocation, StartLocation) > 1))
	{
		FVector NewTarget = FMath::VInterpConstantTo(MeshLocation, StartLocation, DeltaTime, MovementSpeed);
		FRotator NewTargetRotation = FMath::RInterpConstantTo(MeshRotation, StartRotation, DeltaTime, MovementSpeed);
		if (bCheckShouldMove(NewTarget))
		{
			MeshComponent->SetWorldLocationAndRotation(NewTarget, NewTargetRotation);
		}
	}
}

bool ASIA_SecretDoor::TryTrigger_Implementation(AActor* TriggeredBy)
{
	if (CanInteract_Implementation(TriggeredBy))
	{
		Trigger_Implementation(TriggeredBy);
		return true;
	}
	else
	{
		return false;
	}

}

bool ASIA_SecretDoor::Interact_Implementation(AActor* Interactor)
{
	if (bTriggerOnInteract)
	{
		return TryTrigger_Implementation(Interactor);
	}
	else
	{
		return false;
	}
}

bool ASIA_SecretDoor::CanInteract_Implementation(AActor* Interactor)
{
	return true;
}

void ASIA_SecretDoor::Trigger_Implementation(AActor* TriggeredBy)
{
	bIsTriggered = !bIsTriggered;
	/*if (bIsTriggered)
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}*/
}

bool ASIA_SecretDoor::CanTrigger_Implementation(AActor* TriggeredBy)
{
	return true;
}

void ASIA_SecretDoor::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = MeshComponent->GetComponentLocation();
	EndLocation = MoveDestination->GetComponentLocation();

	StartRotation = MeshComponent->GetComponentRotation();
	EndRotation = MoveDestination->GetComponentRotation();
}

bool ASIA_SecretDoor::bCheckShouldMove(FVector LocationToMoveTo)
{
	FVector CurrentLocation = MeshComponent->GetComponentLocation();
	if (CurrentLocation.Z < LocationToMoveTo.Z && bBlockWhenOnTop)
	{
		TArray<AActor*> OverlappingActors;
		TopBoxCollider->GetOverlappingActors(OverlappingActors, ARPGCharacter::StaticClass());
		return OverlappingActors.Num() == 0;
	}
	else if (CurrentLocation.Z > LocationToMoveTo.Z && bBlockWhenBelow)
	{
		TArray<AActor*> OverlappingActors;
		BottomBoxCollider->GetOverlappingActors(OverlappingActors, ARPGCharacter::StaticClass());
		return OverlappingActors.Num() == 0;
	}
	else
	{
		return true;
	}
}
