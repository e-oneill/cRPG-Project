// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInteraction/Selectable.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "GameplayActionSystem/ActionOwnerInterface.h"
#include "RPGCharacter.generated.h"

class UTurn;
class UGameplayActionComponent;
class UInventoryComponent;
class UPawnSensingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterEncounterEventSignature, ARPGCharacter*, Character, AEncounter*, Encounter);

UCLASS(Blueprintable)
class ARPGCharacter : public ACharacter, public ISelectable, public IActionOwnerInterface
{
	GENERATED_BODY()

public:
	ARPGCharacter();

	void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	bool IsActiveTurnCharacter();

	#pragma region Encounter_Events
	UPROPERTY(BlueprintAssignable)
		FCharacterEncounterEventSignature OnEncounterJoined;
	UPROPERTY(BlueprintAssignable)
		FCharacterEncounterEventSignature OnEncounterLeft;
	#pragma endregion Encounter_Events

	#pragma region SelectionInterface
	bool CanSelect_Implementation(ARPGPlayerController* Selector) override;
	void Select_Implementation(ARPGPlayerController* Selector) override;
	void Deselect_Implementation(ARPGPlayerController* Selector) override;
	void SetOnHover_Implementation(bool OnHover) override;
	bool GetOnHover_Implementation() override;
	#pragma  endregion SelectionInterface

	bool IsInEncounter_Implementation() override;

	UPawnSensingComponent* GetSensingComponent() const { return SensingComponent; }
	void SetSensingComponent(UPawnSensingComponent* val) { SensingComponent = val; }

	//UAnimInstance* GetOwnerAnimInstance() override;

protected:
	#pragma region Encounter_State
	UPROPERTY(Replicated)
	UTurn* CharacterTurn;
	#pragma endregion Encounter_state

	#pragma region SelectionInterface
	UPROPERTY(BlueprintReadOnly)
	bool bOnSelectionHover = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection Interface")
	class USelectionDecalComponent* SelectionDecalComponent;


	#pragma endregion SelectionInterface

	#pragma region PlayerData
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPlayerControlComponent* PlayerControlComp;

	
	#pragma endregion PlayerData

	#pragma region SelectionHelpers
	bool bIsSelected;

	void SetDecalMaterial();

#pragma endregion SelectionHelpers

	#pragma region GameplayActionSystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGameplayActionComponent* ActionComponent;
#pragma endregion GameplayActionSystem

	#pragma region InventorySystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	#pragma endregion


	#pragma region AISystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UPawnSensingComponent* SensingComponent;
	#pragma endregion AISystem

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;



	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	UFUNCTION(BlueprintCallable)
		void EndCharacterTurn();

	UFUNCTION(Server, Reliable)
		void Server_EndCharacterTurn();



#pragma region Get_Set
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPlayerControlComponent* GetPlayerControlComp() const { return PlayerControlComp; }

	UFUNCTION(BlueprintPure)
	UTurn* GetCharacterTurn() const;
	void SetCharacterTurn(UTurn* val);

	//ActionOwnerInterface
	UAnimInstance* GetOwnerAnimInstance_Implementation() override;
	APlayerController* GetControllingPlayer_Implementation() override;
#pragma endregion Get_Set

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};

