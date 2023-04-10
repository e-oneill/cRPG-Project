// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/InventorySystemStatics.h"
#include "InventorySystem/InventoryTypes.h"
#include "InventorySystem/InventoryComponent.h"
#include "InventorySystem/ItemBase.h"
#include "InventorySystem/ItemEquippable.h"
#include "InventorySystem/ItemConsumable.h"
#include "InventorySystem/ItemEquippableWeapon.h"

UItemBase* UInventorySystemStatics::AddItemToInventoryFromData(FDataTableRowHandle ItemDataHandle, UInventoryComponent* InventoryToAddTo)
{
	 FInventoryItemData* ItemData = ItemDataHandle.GetRow<FInventoryItemData>(FString(ItemDataHandle.RowName.ToString()));
	 ItemData->RowName = ItemDataHandle.RowName;
	 UItemBase* Item = CreateItemFromData(*ItemData);
	 
	 InventoryToAddTo->AddItemToInventory(Item);
	 return Item;
}

UItemBase* UInventorySystemStatics::CreateItemFromData(const FInventoryItemData& ItemData)
{
	UItemBase* Item;
	switch (ItemData.ItemType)
	{
	case EItemType::Equippable:
		Item = GetCorrectClassOfEquippableItem(ItemData);
		break;
	case EItemType::Consumable:
		Item = NewObject<UItemConsumable>();
		break;
	default:
		Item = NewObject<UItemBase>();
		break;
		
	}
	Item->InitializeItem(ItemData);
	return Item;
}

UItemBase* UInventorySystemStatics::GetCorrectClassOfEquippableItem(const FInventoryItemData& ItemData)
{
	UItemBase* Item;
	switch (ItemData.EquippableType)
	{
	case EEquippableType::Weapon:
		Item = NewObject<UItemEquippableWeapon>();
		break;
	case EEquippableType::Armour:
		//break;
	case EEquippableType::Trinket:
		//break;
	default:
		Item = NewObject<UItemEquippable>();
		break;

	}

	return Item;
}
