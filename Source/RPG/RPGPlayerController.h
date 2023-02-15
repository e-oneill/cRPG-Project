// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "GameplayActionSystem/ActionPlayerControlInterface.h"
#include "RPGPlayerController.generated.h"

DECLARE_DELEGATE_OneParam(FHotbarInputSwitch, const int32);

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UUIManagementComponent;
class AEncounter;
class ISelectable;
class UPlayerControlComponent;
class IInteractable;

UCLASS()
class ARPGPlayerController : public APlayerController, public IActionPlayerControlInterface
{
	GENERATED_BODY()

public:
	#pragma region RPC_Accessors
	/*
		This region is used for methods that exist to allow RPC calls to be sent to the server where the proxy user is not the owner of the actor being called from. 
		For example, this allows the EndTurn function to be called on a character, despite it not being Owned by the proxy player (as Characters are owned by their controller)
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void EndTurnForCharacter(UGameplayActionComponent* InGameplayActionComponent);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipSlotForInventoryWithItem(UInventoryComponent* InventoryToEquip, UItemEquippable* ItemToEquip, EEquipmentSlot SlotToEquipTo);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UnequipSlotForInventory(UInventoryComponent* InventoryToUnequip, EEquipmentSlot SlotToUnequip);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ActorInteractWithInteractable(AActor* Interactor, const TScriptInterface<IInteractable>& Interactable);

	#pragma endregion RPC_Accessors

	ARPGPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	void AddEncounterToUI(AEncounter* Encounter);

	UUIManagementComponent* GetUIManagementComponent() const { return UIManagementComponent; }
	void SetUIManagementComponent(UUIManagementComponent* val) { UIManagementComponent = val; }
	TArray<UPlayerControlComponent*> GetControlled() const { return Controlled; }
	void SetControlled(TArray<UPlayerControlComponent*> val) { Controlled = val; }

	void AddToControlled(UPlayerControlComponent* val) { Controlled.Add(val); }
	void RemoveFromControlled(UPlayerControlComponent* val) {Controlled.Remove(val); }

	TArray<AActor*> GetSelected() const { return Selected; }
	void SetSelected(TArray<AActor*> val) { Selected = val; }


	void Client_PlayActionCue_Implementation(UAction* Action, const FCueConfigurationData& ConfigData, UGameplayActionComponent* TargetComponent, const FVector& TargetLocation) override;

	static ARPGPlayerController* GetWidgetControllerCastToRPGController(UUserWidget* InWidget);

#pragma region ActionExecutionMethods
	//method that is usually called from the client, instructing the server that it wants to prepare an action.
	//later can use this method to do client prediction, to make the process more responsive
	UFUNCTION()
		void TryPrepareAction(UAction* Action, FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION(Server, Unreliable)
		void Server_TryPrepareAction(UAction* Action, const FVector& TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION()
		void TryExecuteAction(UAction* Action, FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION(Server, Unreliable)
		void Server_TryExecuteAction(UAction* Action, const FVector& TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION()
		void CancelAction(UAction* Action);

	UFUNCTION(Server, Unreliable)
		void Server_CancelAction(UAction* Action);

#pragma endregion ActionExecutionMethods

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UUIManagementComponent* UIManagementComponent;

	//an array that contains the currently selected actors
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<AActor*> Selected;

	//an array that contains the actors controlled by this player
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<UPlayerControlComponent*> Controlled;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	#pragma region PlayerInputHandlers
	void OnPrimaryPressed();
	void OnPrimaryReleased();
	void OnSecondaryPressed();
	void OnSecondaryReleased();

	UFUNCTION(BlueprintCallable)
	void OpenInventory();

	UFUNCTION(BlueprintCallable)
	void UseHotBarSkill(const int32 HotBarSlotIndex);

	UFUNCTION(BlueprintCallable)
	bool CanCommandActor(AActor* ActorToCommand);

	UFUNCTION(Server, Reliable)
	void Server_SelectCharacters(const TArray<AActor*>& ToSelects);

	UFUNCTION(Server, Reliable)
	void Server_SelectCharacter(AActor* ToSelect);

	UFUNCTION(Client, Reliable)
	void Client_SelectCharacterCosmetics(AActor* ToSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeselectAll();
	
	UFUNCTION(Server, Reliable)
	void Server_DeselectCharacter(AActor* ToDeselect);

	UFUNCTION(Client, Reliable)
	void Client_DeselectCharacterCosmetics(AActor* ToDeselect);
	
	AActor* GetActorBeneathCursor();

	UPROPERTY(BlueprintReadOnly)
	FVector SelectionStartLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector2D ScreenspaceStartLocation;

	UPROPERTY(BlueprintReadOnly)
	FVector SelectionEndLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector2D ScreenspaceEndLocation;

	UPROPERTY(BlueprintReadOnly)
	float TimePrimaryInputPressed = 0.f;
	UPROPERTY(BlueprintReadOnly)
	bool bPrimaryInputPressed; // Input is bring pressed
	UPROPERTY(BlueprintReadOnly)
	float TimeSecondaryInputPressed = 0.f;
	UPROPERTY(BlueprintReadOnly)
	bool bSecondaryInputPressed; // Input is bring pressed
	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
#pragma endregion PlayerInputHandlers


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};


