// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/ItemBase.h"
#include "Net/UnrealNetwork.h"

void UItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UItemBase, StackSize);
}

