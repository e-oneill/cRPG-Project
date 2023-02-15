// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RPGGameState.generated.h"

class UEncounterManager;
class ARPGCharacter;
class UPlayerControlComponent;
/**
 * 
 */
UCLASS()
class RPG_API ARPGGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARPGGameState();

	void HandleBeginPlay() override;

	TArray<ARPGCharacter*> GetPlayerCharacters();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void EndTurnForCharacter(ARPGCharacter* InCharacter);

	TArray<UPlayerControlComponent*> GetControlledActors() const { return ControlledActors; }
	void SetControlledActors(TArray<UPlayerControlComponent*> val) { ControlledActors = val; }
	void AddToControlledActors(UPlayerControlComponent* In) { ControlledActors.Add(In); }
	void RemoveFromControlledActors(UPlayerControlComponent* Out) { ControlledActors.Remove(Out); }

	#pragma region RandomGeneration
	UFUNCTION(BlueprintPure, Category = "Random Generation")
	FRandomStream GetRandomStream() const { return RandomStream; }
	void SetRandomStream(FRandomStream val) { RandomStream = val; }
	UFUNCTION(BlueprintPure, Category = "Random Generation")
	int32 GetRandomSeed() const { return RandomSeed; }
	UFUNCTION(BlueprintCallable, Category = "Random Generation")
	void SetRandomSeed(int32 val) { RandomSeed = val; }
	UFUNCTION(BlueprintCallable, Category = "Random Generation")
		static int32 GetRandomIntInRange(int32 Min, int32 Max);
	UFUNCTION(BlueprintCallable, Category = "Random Generation")
		static float GetRandomFloatInRange(float Min, float Max);
	UFUNCTION(BlueprintCallable, Category = "Random Generation")
		static FVector GetRandomPointInCone(FVector Direction, float ConeAngle);
	UFUNCTION(BlueprintCallable, Category = "Random Generation")
		static FVector GetRandomPointInBox(FVector Min, FVector Max);
	#pragma endregion RandomGeneration


	UEncounterManager* GetEncounterManager() const { return EncounterManager; }
	void SetEncounterManager(UEncounterManager* val) { EncounterManager = val; }
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<UPlayerControlComponent*> ControlledActors;

	UPROPERTY(Replicated, BlueprintReadOnly)
	UEncounterManager* EncounterManager;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	int32 RandomSeed;

	//UPROPERTY(BlueprintReadOnly, Category = "Random Generation")
	static FRandomStream RandomStream;
	
};
