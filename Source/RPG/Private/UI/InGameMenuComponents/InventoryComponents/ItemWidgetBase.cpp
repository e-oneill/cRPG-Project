// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenuComponents/InventoryComponents/ItemWidgetBase.h"
#include "InventorySystem/ItemEquippable.h"
#include "UI/InventoryUI.h"
#include "InventorySystem/InventoryComponent.h"
#include "../RPGPlayerController.h"
#include "InventorySystem/ItemConsumable.h"

void UItemWidgetBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
}

void UItemWidgetBase::BindToItemEvents()
{
	if (!Item)
	{
		return;
	}

	Item->OnItemUpdate.AddUniqueDynamic(this, &UItemWidgetBase::OnItemUpdate);
	Item->OnItemRemoved.AddUniqueDynamic(this, &UItemWidgetBase::OnItemRemoved);

	if (Cast<UItemEquippable>(Item))
	{
		//this is an equippable item, listen for equipment events.
		UItemEquippable* Equippable = Cast<UItemEquippable>(Item);
		Equippable->OnItemEquipped.AddUniqueDynamic(this, &UItemWidgetBase::OnItemEquip);
		Equippable->OnItemUnequipped.AddUniqueDynamic(this, &UItemWidgetBase::OnItemUnequip);

		//check if item is equipped
		if (Equippable->IsEquipped())
		{
			OnItemEquip(Equippable, Equippable->GetEquipper(), Equippable->GetEquippedSlot());
		}
	}
}

void UItemWidgetBase::UnbindItemEvents()
{
	if (Item)
	{
		Item->OnItemUpdate.RemoveAll(this);
		Item->OnItemRemoved.RemoveAll(this);
	}
}

void UItemWidgetBase::OnItemUpdate_Implementation()
{
	//base implementation does nothing, this function exists so we can add custom handling in both C++ and BP subclasses
	return;
}

void UItemWidgetBase::OnItemRemoved_Implementation()
{
	if (!Item->GetOwningInventory())
	{
		SetItem(nullptr);
	}
}

FReply UItemWidgetBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	ARPGPlayerController* PC = GetOwningPlayer<ARPGPlayerController>();
	if (!PC)
	{
		return Reply;
	}

	//if the item is equippable, try equip it.
	UItemEquippable* ItemEquippable = Cast<UItemEquippable>(Item);
	if (ItemEquippable)
	{
		TryEquipItem(PC, ItemEquippable);
	}
	//if the item is a consumable, consume it.
	UItemConsumable* ItemConsumable = Cast<UItemConsumable>(Item);
	if (ItemConsumable)
	{
		UInventoryComponent* ActiveInventoryComponent = Inventory->GetActiveInventory();
		AActor* OwnerActor = ActiveInventoryComponent->GetOwner();
		TryConsumeItem(PC, ItemConsumable, OwnerActor);
	}

	return Reply;
}

bool UItemWidgetBase::TryEquipItem(ARPGPlayerController* PC, UItemEquippable* ItemEquippable)
{
	
	if (ItemEquippable && Inventory && Inventory->GetActiveInventory())
	{

		EEquipmentSlot SlotToEquip = ItemEquippable->GetValidSlot();
		UInventoryComponent* ActiveInventoryComponent = Inventory->GetActiveInventory();
		//if this item is not equipped, equip it. if it is, unequip.
		if (ActiveInventoryComponent->GetEquippedSlotFromSlotEnum(SlotToEquip).Item != ItemEquippable)
		{
			EquipItemToSlot(ActiveInventoryComponent, PC, ItemEquippable, SlotToEquip);
			return true;
		}
		else
		{
			UnequipItemFromSlot(ActiveInventoryComponent, PC, SlotToEquip);
			return true;
		}

	}
	return false;
}

void UItemWidgetBase::EquipItemToSlot(UInventoryComponent* ActiveInventoryComponent, ARPGPlayerController* PC, UItemEquippable* ItemEquippable, EEquipmentSlot SlotToEquip)
{
	if (ActiveInventoryComponent)
	{
		if (!ActiveInventoryComponent->GetOwner()->HasAuthority())
		{
			//we are on a client, send call to server

			//use the RPC Accessor method on the player controller to trigger this call (there is no controlling connection for characters)
			PC->Server_EquipSlotForInventoryWithItem(ActiveInventoryComponent, ItemEquippable, SlotToEquip);
		}
		//always call locally as well, and server will overwrite result if it thinks is wrong
		ActiveInventoryComponent->EquipSlot(SlotToEquip, ItemEquippable);
	}
}

void UItemWidgetBase::UnequipItemFromSlot(UInventoryComponent* ActiveInventoryComponent, ARPGPlayerController* PC, /*UItemEquippable* ItemEquippable,*/ EEquipmentSlot SlotToEquip)
{
	if (!ActiveInventoryComponent)
	{
		return;
	}

	if (!ActiveInventoryComponent->GetOwner()->HasAuthority())
	{
		//on client, call rpc accessor through player controller
		PC->Server_UnequipSlotForInventory(ActiveInventoryComponent, SlotToEquip);
	}
	ActiveInventoryComponent->UnequipSlot(SlotToEquip);
}

void UItemWidgetBase::OnItemEquip_Implementation(UItemEquippable* EquippedItem, UInventoryComponent* EquippingInventory, const FEquippedSlot& EquippedSlot)
{

}

void UItemWidgetBase::OnItemUnequip_Implementation(UItemEquippable* UnequippedItem)
{

}

UItemWidgetBase* UItemWidgetBase::CreateItemWidget(TSubclassOf<UItemWidgetBase> WidgetClass, APlayerController* InPlayerController, UItemBase* InItem /*= nullptr*/, UInventoryUI* InInventoryUI /*= nullptr*/)
{
	UItemWidgetBase* ItemWidget = CreateWidget<UItemWidgetBase>(InPlayerController, WidgetClass);
	if (ItemWidget)
	{
		ItemWidget->Init(InItem, InInventoryUI);
		return ItemWidget;
	}
	return nullptr;
}

void UItemWidgetBase::Init_Implementation(UItemBase* InItem /*= nullptr*/, UInventoryUI* InInventoryUI /*= nullptr*/)
{
	Item = InItem;
	Inventory = InInventoryUI;
	if (Item)
	{
		BindToItemEvents();
	}
}

void UItemWidgetBase::SetItem(UItemBase* val)
{
	if (Item)
	{
		UnbindItemEvents();
	}

	Item = val;

	Init(Item, Inventory);
}

bool UItemWidgetBase::TryConsumeItem(ARPGPlayerController* PC, UItemConsumable* ItemConsumable, AActor* TargetActor)
{
	if (ItemConsumable)
	{
		ItemConsumable->UseConsumable(TargetActor);
		return true;
	}

	return false;
}
