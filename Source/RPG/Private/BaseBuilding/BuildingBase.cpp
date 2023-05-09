// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBuilding/BuildingBase.h"
#include "Components/InstancedStaticMeshComponent.h"
// Sets default values
ABuildingBase::ABuildingBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	FoundationInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FoundationInstancedMesh"));
	RootComponent = FoundationInstancedMesh;

}

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();

	FTransform MeshTransform = FTransform();

	for (uint8 i = 0; i < 3; i++)
	{
		FoundationInstancedMesh->AddInstance(MeshTransform);
		FVector MeshVector = MeshTransform.GetLocation();
		MeshVector.Z += 250.f;
		MeshTransform.SetLocation(MeshVector);
	}
	

}

void ABuildingBase::DestroyInstance(FVector HitPoint)
{
	const TArray<int32> HitIndexes = FoundationInstancedMesh->GetInstancesOverlappingSphere(HitPoint, 1.f);

	if (HitIndexes.Num() > 0)
	{
		FoundationInstancedMesh->RemoveInstance(HitIndexes[0]);
	}
}

bool ABuildingBase::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("Interacting..."));
	return IInteractable::Execute_CanInteract(this, Interactor);
}

bool ABuildingBase::CanInteract_Implementation(AActor* Interactor)
{
	return false;
}

// Called every frame
/*void ABuildingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

