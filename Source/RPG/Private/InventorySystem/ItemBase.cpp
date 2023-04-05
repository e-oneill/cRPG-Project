// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/ItemBase.h"
#include "Net/UnrealNetwork.h"
#include "InventorySystem/InventoryTypes.h"

void UItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UItemBase, StackSize);
}

void UItemBase::InitializeItem(FInventoryItemData& ItemData)
{
	ItemIcon = ItemData.ItemIcon.Get();
	ItemName = ItemData.ItemName;
	bCanStack = ItemData.bCanStack;
	StackSize = ItemData.CurrStackSize;
	MaxStackSize = ItemData.MaxStackSize;
}

