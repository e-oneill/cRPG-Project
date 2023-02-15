// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UItemBase;
class UItemEquippable;
class UGameplayActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryChanged, UInventoryComponent*, Inventory, UItemBase*, NewItem, UItemBase*, CurrentInventoryItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEquipmentChanged, UInventoryComponent*, Inventory, const FEquippedSlot&, EquipmentSlot, UItemEquippable*, NewItem, UItemEquippable*, OldItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	TArray<UItemBase*> Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, meta = (EditFixedSize))
	TArray<FEquippedSlot> EquipmentSlots;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_NotifyClientsOfItemAdded(UItemBase* NewItem, UItemBase* CurrentInventoryItem);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_NotifyClientsOfItemRemoved(UItemBase* NewItem, UItemBase* CurrentInventoryItem);

	bool UnequipSlotAtIndex(int index);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(UItemBase* Item);
	UFUNCTION(BlueprintCallable)
	void RemoveItemFromInventory(UItemBase* Item);

	UGameplayActionComponent* GetActionComponentFromInventory();

	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//attempts to equip the provided item into the provided slot, return true if equipped, false if not
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	bool EquipSlot(EEquipmentSlot Slot, UItemEquippable* Item);
	//attempt to unequip the given slot, with the option to pass in the item and a bool to enforce that this is indeed the item in slot.
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	bool UnequipSlot(EEquipmentSlot Slot, UItemEquippable* Item = nullptr, bool bEnforceCorrectItem = false);

	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	FEquippedSlot& GetEquippedSlotFromSlotEnum(EEquipmentSlot InSlotEnum);

	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	FEquippedSlot& GetEquippedSlotFromItem(UItemEquippable* InItem);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_NotifyClientsOfEquipmentChange(const FEquippedSlot& EquippedSlot, UItemEquippable* NewItem, UItemEquippable* OldItem);

	UPROPERTY(BlueprintAssignable)
		FOnInventoryChanged OnItemAdded;
	UPROPERTY(BlueprintAssignable)
		FOnInventoryChanged OnItemRemoved;

	UPROPERTY(BlueprintAssignable)
		FOnEquipmentChanged OnEquipmentChanged;

public:
	TArray<UItemBase*> GetInventory() const { return Inventory; }
	void SetInventory(TArray<UItemBase*> val) { Inventory = val; }
};
