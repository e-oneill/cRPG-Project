// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "../RPGCharacter.h"
#include "Turn.generated.h"

class ARPGCharacter;
class AEncounter;
class UActionAttribute;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnEventSignature, UTurn*, Turn);


/**
 * 
 */
UCLASS()
class RPG_API UTurn : public UReplicatedObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ActionComponent)
	UGameplayActionComponent* ActionComponent;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TWeakObjectPtr<AEncounter> Encounter;

	//Turn resources - initial values
	UPROPERTY(Replicated)
	UActionAttribute* ActionPointAttribute;
	UActionAttribute* MovementAttribute;

	UPROPERTY(ReplicatedUsing=OnRep_ActionPoints)
	int32 ActionPoints;
	UPROPERTY(ReplicatedUsing=OnRep_Movement)
	float Movement;

	//Turn resources - tracked values
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_UsedActionPoints)
	int32 UsedActionPoints;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_UsedMovement)
	float UsedMovement;

	UFUNCTION()
	void OnActionPointsChanged(UGameplayActionComponent* OwningComponent, UActionAttribute* Attribute);

	UFUNCTION()
		void OnMovementAttributeChanged(UGameplayActionComponent* OwningComponent, UActionAttribute* Attribute);

public:
	bool IsCurrentTurn();

	void InitializeTurn(UGameplayActionComponent* InActionComponent, TWeakObjectPtr<AEncounter> InEncounter);

	//Turn lifecycle
	void StartTurn();
	void EndTurn();

	void RemoveFromEncounter();

	#pragma region Notification_Events
	//Broadcast turn start event to all clients
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnTurnStart();

	//Broadcast turn end event to all client
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnTurnEnd();

	//on rep for character, that sets the sibling relationship on character (as this does not populate correctly pre-replication
	UFUNCTION()
	void OnRep_ActionComponent();

	UFUNCTION()
	void OnRep_ActionPoints();

	UFUNCTION()
	void OnRep_Movement();

	//Broadcast Used Action Points Event
	UFUNCTION()
	void OnRep_UsedActionPoints();

	//Broadcast Movement Points Event
	UFUNCTION()
	void OnRep_UsedMovement();

	//Turn events
	
	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnTurnStart;
	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnTurnEnd;
	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnActionPointsChange;
	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnMovementChanged;
	#pragma endregion Notification_Events


	#pragma region Get_Set
	UGameplayActionComponent* GetActionComponent() const { return ActionComponent; }
	void SetActionComponent(UGameplayActionComponent* val) { ActionComponent = val; }


	//UFUNCTION(BlueprintPure, Category = "Turns")
	TWeakObjectPtr<AEncounter> GetEncounter() const { return Encounter; }
	//UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetEncounter(TWeakObjectPtr<AEncounter> val) { Encounter = val; }

	UFUNCTION(BlueprintPure, Category = "Turns")
	int32 GetActionPoints() const { return ActionPoints; }
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetActionPoints(int32 val);

	UFUNCTION(BlueprintPure, Category = "Turns")
	float GetMovement() const { return Movement; }
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetMovement(float val);

	UFUNCTION(BlueprintPure, Category = "Turns")
	int32 GetUsedActionPoints() const { return UsedActionPoints; }
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetUsedActionPoints(int32 val);
	UFUNCTION(BlueprintPure, Category = "Turns")
	int32 GetRemainingActionPoints() { return ActionPoints - UsedActionPoints; }

	UFUNCTION(BlueprintPure, Category = "Turns")
	float GetUsedMovement() const { return UsedMovement; }
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetUsedMovement(float val);

	UFUNCTION(BlueprintPure, Category = "Turns")
	float GetRemainingMovement() { return (Movement - UsedMovement); }

	#pragma endregion Get_Set


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UActionAttribute* GetMovementAttribute() const { return MovementAttribute; }
	void SetMovementAttribute(UActionAttribute* val) { MovementAttribute = val; }
};
