// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystem/InventoryTypes.h"
#include "InventoryUI.generated.h"

class UInventoryComponent;
class UScrollBox;
class UEquipmentSlotWidget;
class UItemEquippable;
class UWrapBox;
class UItemWidgetBase;
class UVerticalBox;
class UGameMenuCharacterSwitch;
class UGameplayActionComponent;
class ARPGPlayerController;

/**
 * 
 */
UCLASS()
class RPG_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()

public:
	static UInventoryUI* CreateInventoryUI(APlayerController* PlayerController, TSubclassOf<UInventoryUI> InventoryWidgetClass, UInventoryComponent* InInventory);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(UInventoryComponent* InventoryComponent);

	void PostInitProperties() override;

	bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	UInventoryComponent* ActiveInventory;

	#pragma region Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UScrollBox* ItemTableContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UWrapBox* InventorySlotContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UVerticalBox* ControlledCharactersContainer;

	#pragma endregion Widgets

	#pragma region EquipmentSlots
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* MainHandSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* OffHandSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* HeadSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* NeckSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* BootSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* BodySlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* GlovesSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* BeltSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UEquipmentSlotWidget* TrinketSlot;

	UPROPERTY(VisibleInstanceOnly)
	TArray<UEquipmentSlotWidget*> SlotWidgets;

	#pragma endregion EquipmentSlots
	
	#pragma region WidgetClasses
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameMenuCharacterSwitch> SwitchClass;

	//an array that holds the equipment slot widgets
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemWidgetBase> InventorySlotWidget;

	#pragma endregion WidgetClasses

	UPROPERTY(EditDefaultsOnly)
	int32 DefaultInventorySlots = 30;

	#pragma region Widget Setup
	void SetupEquipmentSlots();

	void SetupEquipmentSlot(UEquipmentSlotWidget* SlotToSetup);

	void SetupCharacterSwitchWidgets(ARPGPlayerController* RPGPlayerController);
	#pragma endregion Widget Setup

	void NativeConstruct() override;

	UFUNCTION()
	void OnEquipmentChanged(UInventoryComponent* Inventory, const FEquippedSlot& EquipmentSlot, UItemEquippable* NewItem, UItemEquippable* OldItem);

	void DeactivateCurrentSwitch();

public:
	UInventoryComponent* GetActiveInventory() const { return ActiveInventory; }
	void SetActiveInventory(UInventoryComponent* val) { ActiveInventory = val; }

	UEquipmentSlotWidget* GetEquipmentSlotWidgetFromSlot(EEquipmentSlot InSlot);

	UFUNCTION()
		void HandleCharacterSwitch(UGameMenuCharacterSwitch* Switch, AActor* SwitchCharacter, UGameplayActionComponent* SwitchActionComponent, UInventoryComponent* SwitchInventoryComponent);

	void SetupInventorySlots();
	


		

};
