// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystem/InventoryTypes.h"
#include "DragAndDrop/DraggableWidget.h"
#include "InventoryComponents/ItemWidgetBase.h"
#include "ItemTableRow.generated.h"

class UItemBase;
class UTextBlock;
class UInventoryUI;
class UInventoryComponent;
class ARPGPlayerController;
class UItemEquippable;

/**
 * 
 */
UCLASS()
class RPG_API UItemTableRow : public UItemWidgetBase
{
	GENERATED_BODY()

protected:
	

	void NativeConstruct() override;

	
#pragma region ItemDetails
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemCount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemWeight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemStackWeight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemUnitValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
		UTextBlock* ItemStackValue;
#pragma endregion ItemDetails

	

public:
	
};
