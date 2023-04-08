// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/ItemEquippable.h"
#include "InventorySystem/InventoryComponent.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "../RPG.h"

void UItemEquippable::OnEquip_Implementation(UInventoryComponent* InEquipper, FEquippedSlot& InEquippedSlot)
{
	Equipper = InEquipper;
	bIsEquipped = true;
	//EquippedSlot = InEquippedSlot;
	UGameplayActionComponent* EquipperActionComponent = InEquipper->GetActionComponentFromInventory();
	if (!EquipperActionComponent)
	{
		UE_LOG(LogRPG, Error, TEXT("%s: Tried to equip item on an actor that does not have an action component. The inventory system system relies on the action system to grant actions"), *InEquipper->GetOwner()->GetName());
		return;
	}
	

	ItemStaticMeshComponent = NewObject<UStaticMeshComponent>(EquipperActionComponent->GetOwner(), UStaticMeshComponent::StaticClass(), (TEXT("%s-%s-%s"), *Equipper->GetOwner()->GetName(), *GetName(), *ItemStaticMesh->GetName()));
	if (ItemStaticMeshComponent)
	{
		ItemStaticMeshComponent->SetStaticMesh(ItemStaticMesh);
		ItemStaticMeshComponent->RegisterComponent();
		ItemStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemStaticMeshComponent->SetSimulatePhysics(false);
		ItemStaticMeshComponent->bUseAttachParentBound = true;
		//ItemStaticMeshComponent->Shadow
		//get the skeletal mesh for the equipper and attach to a socket on it.
		USkeletalMeshComponent* EquipperSkeletalMesh = Cast<USkeletalMeshComponent>(Equipper->GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (EquipperSkeletalMesh)
		{
			FAttachmentTransformRules AttachTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
			AttachTransformRules.ScaleRule = EAttachmentRule::KeepWorld;
			ItemStaticMeshComponent->AttachToComponent(EquipperSkeletalMesh, AttachTransformRules, AttachSocket);
		}
	}


	for (TSubclassOf<UAction> GrantsActionClass : GrantsActions)
	{
		UAction* GrantedAction = EquipperActionComponent->GrantAction(GrantsActionClass);
		GrantedActions.Add(GrantedAction);
	}

	OnItemEquipped.Broadcast(this, Equipper, InEquippedSlot);
}
 
void UItemEquippable::OnUnequip_Implementation()
{
	bIsEquipped = false;
	if (!Equipper)
	{
		return;
	}
	
	UGameplayActionComponent* EquipperActionComponent = Equipper->GetActionComponentFromInventory();
	if (!EquipperActionComponent)
	{
		UE_LOG(LogRPG, Error, TEXT("%s: Tried to equip item on an actor that does not have an action component. The inventory system system relies on the action system to grant actions"), *Equipper->GetOwner()->GetName());
		return;
	}

	if (ItemStaticMeshComponent)
	{
		ItemStaticMeshComponent->DestroyComponent(false);
	}

	for (int i = 0; i < GrantedActions.Num(); i++)
	{
		EquipperActionComponent->RemoveAction(GrantedActions[i]);
	}

	OnItemUnequipped.Broadcast(this);
}

FEquippedSlot& UItemEquippable::GetEquippedSlot()
{
	//if this is called on an unequipped item, we'll get a fatal error
	checkf(Equipper != nullptr, TEXT("Tried to get equipped slot on item %s that had no equipper - this will cause a fatal error"), GetNameForUI());



	return Equipper->GetEquippedSlotFromItem(this);
}

void UItemEquippable::InitializeItem(const FInventoryItemData& ItemData)
{
	Super::InitializeItem(ItemData);

	ValidSlot = ItemData.ValidSlot;
	ItemStaticMesh = ItemData.StaticMesh.LoadSynchronous();
	AttachSocket = ItemData.AttachSocket;

	for (TSoftClassPtr<UAction> ItemClass : ItemData.GrantsActions)
	{
		GrantsActions.Add(ItemClass.Get());
	}

	
}
