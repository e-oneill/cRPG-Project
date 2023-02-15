// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EncounterManager.generated.h"

class AEncounter;
class ARPGCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UEncounterManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEncounterManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<AEncounter*> Encounters;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	AEncounter* StartEncounterWithPlayerCharacters();
	UFUNCTION(BlueprintCallable)
	AEncounter* StartEncounter(TArray<ARPGCharacter*> Characters);
	UFUNCTION(BlueprintCallable)
	void EndEncounter(AEncounter* InEncounter);

	TArray<AEncounter*> GetEncounters() const { return Encounters; }
	void SetEncounters(TArray<AEncounter*> val) { Encounters = val; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
