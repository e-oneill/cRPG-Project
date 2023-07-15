// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentalTriggerActor.generated.h"

class UBoxComponent;
class UDecalComponent;

UCLASS()
class RPG_API AEnvironmentalTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentalTriggerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger")
	bool bIsVisible;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environmental Trigger")
	UBoxComponent* TriggerCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environmental Trigger")
	UDecalComponent* SpottedDecal;

	UFUNCTION()
	void OnColliderOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure, Category = "Environmental Trigger")
	bool GetIsVisible() const {return bIsVisible;}

	UFUNCTION(BlueprintCallable, Category = "Environmental Trigger")
	void SetIsVisible(bool bInIsVisible);

	UFUNCTION(BlueprintCallable, Category = "Environmental Trigger")
	void TriggerActor(UGameplayActionComponent* TriggeringActor);

};
