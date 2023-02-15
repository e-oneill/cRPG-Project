// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenuComponents/InventoryComponents/EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "InventorySystem/ItemEquippable.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UEquipmentSlotWidget::SetEquipmentItem(TWeakObjectPtr<UItemEquippable> val)
{
	EquipmentItem = val;
	UItemEquippable* DepointeredItem = nullptr;
	if (EquipmentItem.IsValid())
	{
		DepointeredItem = EquipmentItem.Get();

		FSlateBrush& IconBrush = EquipmentIcon->Brush;
		UWidgetBlueprintLibrary::SetBrushResourceToTexture(IconBrush, DepointeredItem->GetImageForUI());
		EquipmentIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		//we are unequipping
		EquipmentIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	OnEquipmentItemUpdate(DepointeredItem);
	

}