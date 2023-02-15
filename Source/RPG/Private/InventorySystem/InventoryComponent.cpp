// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "InventorySystem/ItemBase.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "InventorySystem/ItemEquippable.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	for (EEquipmentSlot SlotEnum : TEnumRange<EEquipmentSlot>())
	{
		FEquippedSlot Slot;
		Slot.Slot = SlotEnum;
		Slot.Item = nullptr;
		EquipmentSlots.Add(Slot);
	}


	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UInventoryComponent::NetMulticast_NotifyClientsOfItemAdded_Implementation(UItemBase* NewItem, UItemBase* CurrentInventoryItem)
{
	OnItemAdded.Broadcast(this, NewItem, CurrentInventoryItem);
	NewItem->OnItemAdded.Broadcast();
}

void UInventoryComponent::NetMulticast_NotifyClientsOfItemRemoved_Implementation(UItemBase* NewItem, UItemBase* CurrentInventoryItem)
{
	NewItem->OnItemRemoved.Broadcast();
	OnItemRemoved.Broadcast(this, NewItem, CurrentInventoryItem);
}



// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::AddItemToInventory(UItemBase* Item)
{
	//check if item can stack, if so find whether one of it is already in the inventory
	if (Item->CanStack() == true)
	{
		//item can stack, check if we have the same class of item in our inventory already
		for (UItemBase* InventoryItem : Inventory)
		{
			//if we already have an item of the same class, iterate the stack
			if (InventoryItem->GetClass() == Item->GetClass())
			{
				//this item is the same class as the one that we want to add
				if (InventoryItem->GetStackSize() < InventoryItem->GetMaxStackSize())
				{
					InventoryItem->SetStackSize(InventoryItem->GetStackSize() + Item->GetStackSize());
					NetMulticast_NotifyClientsOfItemAdded(Item, InventoryItem);
					return;
				}
			}
		}
		Item->SetOwningInventory(this);
		Inventory.Add(Item);
		NetMulticast_NotifyClientsOfItemAdded(Item, Item);
	}
	else
	{
		Item->SetOwningInventory(this);
		Inventory.Add(Item);
		NetMulticast_NotifyClientsOfItemAdded(Item, Item);
	}

	
}

void UInventoryComponent::RemoveItemFromInventory(UItemBase* Item)
{
	//check if item can stack, and if so see if we need to just decrease the stack or remove it
	if (!Inventory.Contains(Item))
	{
		return;
	}

	if (Item->CanStack() == true && Item->GetStackSize() > 1)
	{
		Item->SetStackSize(Item->GetStackSize() - 1);
		NetMulticast_NotifyClientsOfItemRemoved(Item, Item);
	}
	else
	{
		Inventory.Remove(Item);
		Item->SetOwningInventory(nullptr);
		NetMulticast_NotifyClientsOfItemRemoved(Item, nullptr);
	}
}

UGameplayActionComponent* UInventoryComponent::GetActionComponentFromInventory()
{
	UGameplayActionComponent* GAC = Cast<UGameplayActionComponent>(GetOwner()->GetComponentByClass(UGameplayActionComponent::StaticClass()));

	return GAC;
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	bWroteSomething |= Channel->ReplicateSubobjectList(Inventory, *Bunch, *RepFlags);

	return bWroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, EquipmentSlots);
}

bool UInventoryComponent::EquipSlot(EEquipmentSlot Slot, UItemEquippable* Item)
{
	for (int i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].Slot == Slot)
		{
			UItemEquippable* OldItem = nullptr;
			if (EquipmentSlots[i].Item)
			{
				OldItem = EquipmentSlots[i].Item;
				UnequipSlotAtIndex(i);
				//EquipmentSlots[i].Item->UnequipItem();
			}
			EquipmentSlots[i].Item = Item;
			Item->OnEquip(this, EquipmentSlots[i]);
			//EquipmentSlots[i].ItemData = Item->GetItemData();
			NetMulticast_NotifyClientsOfEquipmentChange(EquipmentSlots[i], Item, OldItem);
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::UnequipSlot(EEquipmentSlot Slot, UItemEquippable* Item /*= nullptr*/, bool bEnforceCorrectItem /*= false*/)
{
	for (int i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].Slot != Slot)
		{
			continue;
		}
		if (EquipmentSlots[i].Item)
		{
			UItemEquippable* OldItem = EquipmentSlots[i].Item;
			if (Item && bEnforceCorrectItem && OldItem != Item)
			{
				return false;
			}
			UnequipSlotAtIndex(i);
			NetMulticast_NotifyClientsOfEquipmentChange(EquipmentSlots[i], nullptr, OldItem);
			return true;
		}
	}

	return false;
}



FEquippedSlot& UInventoryComponent::GetEquippedSlotFromSlotEnum(EEquipmentSlot InSlotEnum)
{
	for (FEquippedSlot& Slot : EquipmentSlots)
	{
		if (Slot.Slot == InSlotEnum)
		{
			return Slot;
		}
	}
	//if we didn't find it just return the first - shouldn't ever happen
	UE_LOG(LogRPG, Error, TEXT("Tried to get an equipped slot that did not exist on this Inventory Component"));
	return EquipmentSlots[0];
}

FEquippedSlot& UInventoryComponent::GetEquippedSlotFromItem(UItemEquippable* InItem)
{
	for (FEquippedSlot& Slot : EquipmentSlots)
	{
		if (Slot.Item == InItem)
		{
			return Slot;
		}
	}
	//if we didn't find it just return the first - shouldn't ever happen
	UE_LOG(LogRPG, Error, TEXT("Tried to get an equipped slot for an item that was not equipped"));
	return EquipmentSlots[0];
}

bool UInventoryComponent::UnequipSlotAtIndex(int index)
{
	if (EquipmentSlots.Num() > index + 1)
	{
		EquipmentSlots[index].Item->OnUnequip();
		EquipmentSlots[index].Item = nullptr;
		
		return true;
	}
	
	return false;
}

void UInventoryComponent::NetMulticast_NotifyClientsOfEquipmentChange_Implementation(const FEquippedSlot& EquippedSlot, UItemEquippable* NewItem, UItemEquippable* OldItem)
{
	//due to how replication handles arrays of structs, we have to manually replicate the equipment slot to be the right slot. 
	//do not need to do this on the host, as it will already be done
	if (!GetOwner()->HasAuthority())
	{
		for (int i = 0; i < EquipmentSlots.Num(); i++)
		{
			if (EquipmentSlots[i].Slot == EquippedSlot.Slot)
			{
				EquipmentSlots[i].Item = EquippedSlot.Item;
				EquipmentSlots[i].ItemData = EquippedSlot.ItemData;
				break;
			}
		}
	}

	OnEquipmentChanged.Broadcast(this, EquippedSlot, NewItem, OldItem);



	
}

