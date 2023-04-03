// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turn.h"
#include "Encounter.generated.h"


class ARPGCharacter;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnEventSignature, UTurn*, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEncounterEventSignature, AEncounter*, OnEncounter);

UCLASS()
class RPG_API AEncounter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<UTurn*> Turns;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTurn, BlueprintReadOnly)
	UTurn* CurrentTurn;

	UPROPERTY(Replicated)
	int32 CurrentTurnIndex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Start();
	void EndCurrentTurn();
	UFUNCTION(BlueprintCallable)
	void End();

	void AddCharacters(TArray<ARPGCharacter*> Characters);
	bool IsCharacterInEncounter(ARPGCharacter* Character);
	bool IsCharacterInEncounter(UGameplayActionComponent* ActionComponent);

	void AddActionComponents(TArray<UGameplayActionComponent*> ActionComponents);

	void AddCharacterToEncounter(ARPGCharacter* InCharacter);
	void AddCharacterToEncounter(UGameplayActionComponent* InActionComponent);
	void RemoveCharacterFromEncounter(ARPGCharacter* InCharacter);

#pragma region Network_EventDispatchers
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnCharacterJoinEncounter(ARPGCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnCharacterLeftEncounter(ARPGCharacter* Character);

	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnCurrentTurnChanged;

	UPROPERTY(BlueprintAssignable)
	FCharacterEncounterEventSignature OnCharacterJoinEncounter;

	UPROPERTY(BlueprintAssignable)
	FEncounterEventSignature OnEncounterStart;

	UPROPERTY(BlueprintAssignable)
	FEncounterEventSignature OnEncounterEnd;

	UFUNCTION(NetMulticast,Reliable)
	void NetMulticast_OnEncounterStart();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_OnEncounterEnd();

	UFUNCTION()
	void OnRep_CurrentTurn();
#pragma endregion Network_EventDispatchers

	void AddEncounterToUI();


	TArray<UTurn*> GetTurns() const { return Turns; }
	void SetTurns(TArray<UTurn*> val) { Turns = val; }

	void AddTurn(UTurn* InTurn);
	


	UTurn* GetCurrentTurn() const { return CurrentTurn; }
	void SetCurrentTurn(UTurn* val) { CurrentTurn = val; }


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
