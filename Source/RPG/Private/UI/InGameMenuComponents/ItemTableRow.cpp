// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenuComponents/ItemTableRow.h"
#include "InventorySystem/ItemEquippable.h"
#include "UI/InventoryUI.h"
#include "InventorySystem/InventoryComponent.h"
#include "../RPGPlayerController.h"

void UItemTableRow::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindToItemEvents();

}
