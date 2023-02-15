// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticInteractableActor.h"
#include "Components/StaticMeshComponent.h"

AStaticInteractableActor::AStaticInteractableActor() : Super()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
}
