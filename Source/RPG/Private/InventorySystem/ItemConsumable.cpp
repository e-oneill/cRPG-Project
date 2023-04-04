// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/ItemConsumable.h"
#include "../RPG.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "InventorySystem/InventoryComponent.h"

void UItemConsumable::UseConsumable_Implementation(AActor* TargetActor, FVector TargetLocation /*= FVector::ZeroVector*/)
{
	/*if (!GetOuter()->HasAuthority())
	{
		UE_LOG(LogRPG, Error, TEXT("Consumable item attempted to use from client, this should be executed on the server"));
		return;
	}*/

	//Uses--;
	//Create consumable action and apply it to the target.
	if (!ConsumableAction)
	{
		ConsumableAction = NewObject<UAction>(this, ConsumableActionType);
	}
	
	UGameplayActionComponent* TargetActionComponent = Cast<UGameplayActionComponent>(TargetActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (TargetActionComponent && ConsumableAction)
	{
		ConsumableAction->InitializeAction(TargetActionComponent);
		if (ConsumableAction->TryExecuteAction(TargetLocation, TargetActor))
		{
			Uses--;
		}
	}

	if (Uses <= 0)
	{
		
		//consumable is used up
		//if action is active, remove from owning inventory and destroy when action is finished.
		//if action is not active, remove from owning inventory and destroy now
		if (OwningInventory)
		{
			OwningInventory->RemoveItemFromInventory(this);
		}
		//if the consumable is still in the owning inventory, reset uses back to initial value - this means that the stack size is greater than 
		if (OwningInventory)
		{
			Uses = InitialUses;
		}
	}

	
}

void UItemConsumable::PostInitProperties()
{
	Super::PostInitProperties();
	InitialUses = Uses;
}

