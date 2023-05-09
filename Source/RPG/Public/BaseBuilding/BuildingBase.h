// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PlayerInteraction/Interactable.h"
#include "BuildingBase.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class RPG_API ABuildingBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base Building")
	UInstancedStaticMeshComponent* FoundationInstancedMesh;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	bool CanInteract_Implementation(AActor* Interactor) override;
	bool Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Base Building")
	void DestroyInstance(FVector HitPoint);


};
