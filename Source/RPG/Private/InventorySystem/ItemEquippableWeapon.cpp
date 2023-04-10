// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/ItemEquippableWeapon.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Actions/WeaponAction.h"

void UItemEquippableWeapon::InitializeItem(const FInventoryItemData& ItemData)
{
	Super::InitializeItem(ItemData);

	SetDamageRating(ItemData.DamageRating);
	for (TSoftClassPtr<UWeaponAction> WeaponClassSoftPtr : ItemData.WeaponActions)
	{
		WeaponActionClasses.Add(WeaponClassSoftPtr.LoadSynchronous());
	}

}

void UItemEquippableWeapon::GrantActions(UGameplayActionComponent* EquipperActionComponent)
{
	Super::GrantActions(EquipperActionComponent);

	for (TSubclassOf<UWeaponAction> WeaponActionClass : WeaponActionClasses)
	{
		UWeaponAction* WeaponAction = UWeaponAction::CreateWeaponAction(this, WeaponActionClass, EquipperActionComponent);
		EquipperActionComponent->GrantAction(WeaponAction);
		GrantedActions.Add(WeaponAction);
	}
}
