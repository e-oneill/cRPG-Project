// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystem/InventoryTypes.h"
#include "EquipmentSlotWidget.generated.h"

class UImage;
class UItemEquippable;

/**
 * 
 */
UCLASS()
class RPG_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget = true))
	UImage* EquipmentIcon;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UItemEquippable> EquipmentItem;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	EEquipmentSlot EquipmentSlot;
public:

	TWeakObjectPtr<UItemEquippable> GetEquipmentItem() const { return EquipmentItem; }
	void SetEquipmentItem(TWeakObjectPtr<UItemEquippable> val);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipmentItemUpdate(UItemEquippable* NewItem);

	EEquipmentSlot GetEquipmentSlot() const { return EquipmentSlot; }
	void SetEquipmentSlot(EEquipmentSlot val) { EquipmentSlot = val; }
};
