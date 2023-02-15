// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragAndDrop/DraggableWidget.h"
#include "InventorySystem/InventoryTypes.h"
#include "ItemWidgetBase.generated.h"

class UItemBase;
class UTextBlock;
class UInventoryUI;
class UInventoryComponent;
class ARPGPlayerController;
class UItemEquippable;
class UItemConsumable;

/**
 * 
 */
UCLASS()
class RPG_API UItemWidgetBase : public UDraggableWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
		void BindToItemEvents();

		void UnbindItemEvents();


		UFUNCTION(BlueprintNativeEvent)
		void OnItemUpdate();
		UFUNCTION(BlueprintNativeEvent)
		void OnItemRemoved();


	#pragma region ItemVariables
	UPROPERTY(BlueprintReadWrite, Category = "Inventory System", meta = (ExposeOnSpawn = true))
		UItemBase* Item;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory System", meta = (ExposeOnSpawn = true))
		UInventoryUI* Inventory;
	#pragma endregion ItemVariables
	
	#pragma region DragAndDrop/Interaction
	void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	bool TryEquipItem(ARPGPlayerController* PC, UItemEquippable* ItemEquippable);
	bool TryConsumeItem(ARPGPlayerController* PC, UItemConsumable* ItemConsumable, AActor* TargetActor);

#pragma endregion DragAndDrop/Interaction
	
	#pragma region Equipping
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
		void EquipItemToSlot(UInventoryComponent* ActiveInventoryComponent, ARPGPlayerController* PC, UItemEquippable* ItemEquippable, EEquipmentSlot SlotToEquip);
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
		void UnequipItemFromSlot(UInventoryComponent* ActiveInventoryComponent, ARPGPlayerController* PC,/* UItemEquippable* ItemEquippable,*/ EEquipmentSlot SlotToEquip);
	UFUNCTION(BlueprintNativeEvent)
		void OnItemEquip(UItemEquippable* EquippedItem, UInventoryComponent* EquippingInventory, const FEquippedSlot& EquippedSlot);

	UFUNCTION(BlueprintNativeEvent)
		void OnItemUnequip(UItemEquippable* UnequippedItem);
	#pragma endregion Equipping

public:
	static UItemWidgetBase* CreateItemWidget(TSubclassOf<UItemWidgetBase> WidgetClass, APlayerController* InPlayerController, UItemBase* InItem = nullptr, UInventoryUI* InInventoryUI = nullptr);
	UFUNCTION(BlueprintNativeEvent)
	void Init(UItemBase* InItem = nullptr, UInventoryUI* InInventoryUI = nullptr);
	UFUNCTION(BlueprintPure)
	UItemBase* GetItem() const { return Item; }
	UFUNCTION(BlueprintCallable)
	void SetItem(UItemBase* val);

	
};
