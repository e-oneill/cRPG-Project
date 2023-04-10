// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/ItemEquippable.h"
#include "ItemEquippableWeapon.generated.h"

class UGameplayActionComponent;

/**
 * 
 */
UCLASS()
class RPG_API UItemEquippableWeapon : public UItemEquippable
{
	GENERATED_BODY()

public:
	void InitializeItem(const FInventoryItemData& ItemData) override;


	float GetDamageRating() const { return DamageRating; }
	void SetDamageRating(float val) { DamageRating = val; }
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Inventory System|Weapon")
	float DamageRating;

	void GrantActions(UGameplayActionComponent* EquipperActionComponent) override;

	UPROPERTY()
	TArray<TSubclassOf<UWeaponAction>> WeaponActionClasses;
};
