// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Triggerable.h"
#include "EnvironmentalHazard.generated.h"


class UGameplayActionComponent;
class UAction;

UCLASS()
class RPG_API AEnvironmentalHazard : public AActor, public ITriggerable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentalHazard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Hazard")
	UGameplayActionComponent* ActionComponent;

	UPROPERTY(EditAnywhere, Category = "Hazard")
	TArray<TSubclassOf<UAction>> HazardActionClasses;

	UPROPERTY(VisibleInstanceOnly, Category = "Hazard")
	TArray<UAction*> HazardActions;

	UPROPERTY(EditAnywhere, Category = "Hazard")
		bool bAllowRetrigger;

	UPROPERTY(VisibleInstanceOnly, Category = "Hazard")
	bool bTriggered;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	bool CanTrigger_Implementation(AActor* TriggeredBy) override;


	bool TryTrigger_Implementation(AActor* TriggeredBy) override;


	void Trigger_Implementation(AActor* TriggeredBy) override;

};
