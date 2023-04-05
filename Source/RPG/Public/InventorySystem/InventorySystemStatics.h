// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "InventoryTypes.h"
#include "InventorySystemStatics.generated.h"

class UInventoryComponent;

/**
 * 
 */
UCLASS()
class RPG_API UInventorySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UItemBase* AddItemToInventoryFromData(FDataTableRowHandle ItemDataHandle, UInventoryComponent* InventoryToAddTo);
	UFUNCTION(BlueprintCallable)
	static UItemBase* CreateItemFromData(const FInventoryItemData& ItemData);
	
};
