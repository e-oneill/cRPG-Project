// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/ItemBase.h"
#include "InventoryTypes.h"
#include "ItemEquippable.generated.h"

class UInventoryComponent;
class UAction;
struct FEquippedSlot;
struct FEquippedSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemEquip, UItemEquippable*, EquippedItem, UInventoryComponent*, EquippingInventory, const FEquippedSlot&, EquippedSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnequip, UItemEquippable*, UnequippedItem);
/**
 * 
 */
UCLASS()
class RPG_API UItemEquippable : public UItemBase
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory System")
	bool bIsEquipped;

	//FEquippedSlot EquippedSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory System")
	EEquipmentSlot ValidSlot;

	UInventoryComponent* Equipper;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory System")
	TArray<TSubclassOf<UAction>> GrantsActions;

	TArray<UAction*> GrantedActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory System")
	UStaticMesh* ItemStaticMesh;

	UStaticMeshComponent* ItemStaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachSocket = FName("hand_rSocket");

public:
	EEquipmentSlot GetValidSlot() const { return ValidSlot; }
	void SetValidSlot(EEquipmentSlot val) { ValidSlot = val; }

	UFUNCTION(BlueprintNativeEvent)
	void OnEquip(UInventoryComponent* InEquipper, FEquippedSlot& InEquippedSlot);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnequip();

	UInventoryComponent* GetEquipper() const { return Equipper; }
	FEquippedSlot& GetEquippedSlot(); 
	//void SetEquipper(UInventoryComponent* val) { Equipper = val; }
	UStaticMesh* GetItemStaticMesh() const { return ItemStaticMesh; }
	void SetItemStaticMesh(UStaticMesh* val) { ItemStaticMesh = val; }
	bool IsEquipped() const { return bIsEquipped; }
	void IsEquipped(bool val) { bIsEquipped = val; }
	
	UPROPERTY(BlueprintAssignable)
	FOnItemEquip OnItemEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnItemUnequip OnItemUnequipped;

	
	

	void InitializeItem(const FInventoryItemData& ItemData) override;

};
