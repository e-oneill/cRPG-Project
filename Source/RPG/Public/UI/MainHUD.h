// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

class UEncounterTracker;
class AEncounter;
class UHorizontalBox;
class UAction;
class UHotBarButton;
class UGameplayActionComponent;
class UActionAttribute;
class UButton;
class UVerticalBox;
class UUI_ControlledCharAvatar;
class UInventoryComponent;
class UItemChangeBlock;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedActorChanged, UGameplayActionComponent*, SelectedActor);

/**
 * 
 */
UCLASS(Blueprintable)
class RPG_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Hot Bar")
		TArray<UHotBarButton*> HotBarButtons;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEncounterTracker* EncounterTracker;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UHorizontalBox* ActionPointsContainer;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UHorizontalBox* HotBarContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UVerticalBox* ItemPopupsContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UButton* EndTurnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UVerticalBox* CharacterAvatarsContainer;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUI_ControlledCharAvatar> CharAvatarClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemChangeBlock> ItemPopupClass;


	void NativeConstruct() override;

	AActor* SelectedActor;

	bool bControlledByPlayer;

	UTurn* Turn;

	UFUNCTION(BlueprintNativeEvent)
	void OnTurnAssigned(UTurn* InTurn);
	UFUNCTION(BlueprintNativeEvent)
	void OnTurnEnd(UTurn* InTurn);

	UPROPERTY(BlueprintReadOnly)
	UGameplayActionComponent* SelectedComponent;

	UPROPERTY(BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;

	//Event fired when the movement attribute changes, use to update UI elements related to Movement
	UFUNCTION(BlueprintNativeEvent)
	void OnMovementChanged(UGameplayActionComponent* InActionComponent, UActionAttribute* InMovementAttr);

	UFUNCTION(BlueprintNativeEvent)
	void OnAPChanged(UGameplayActionComponent* InActionComponent, UActionAttribute* InAPAttr);

	UFUNCTION()
	void OnPressEndTurn();

	void SetupActionPoints(UGameplayActionComponent* ActionComponent, UActionAttribute* APAttribute);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSelectedActorChanged(AActor* InSelectedActor);

public:
	void HandleSelectedActorChange(AActor* InSelectedActor);

	void AssignBoundActionsToHotbar(UGameplayActionComponent* ActionComponent);

	void UpdateSelectedActorEventBindings(AActor* InSelectedActor);
	
	void AddTurnTrackersToScreen(AEncounter* ActiveEncounter);

	UFUNCTION()
	void AssignActionToHotBar(UAction* Action, int32 HotBarSlotIndex);
	UFUNCTION()
	void OnItemAdded(UInventoryComponent* Inventory, UItemBase* NewItem, UItemBase* CurrentInventoryItem);
	UFUNCTION()
	void OnItemRemoved(UInventoryComponent* Inventory, UItemBase* NewItem, UItemBase* CurrentInventoryItem);

	int32 GetNextFreeActionSlot();

};
