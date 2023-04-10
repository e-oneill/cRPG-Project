// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/Actions/WeaponAction.h"
#include "InventorySystem/ItemEquippableWeapon.h"
#include "ActionSystemTags.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

UWeaponAction* UWeaponAction::CreateWeaponAction(UItemEquippableWeapon* InWeapon, TSubclassOf<UWeaponAction> WeaponActionClass, UGameplayActionComponent* InSource, bool bInitialize /*= false*/)
{
	UWeaponAction* WeaponAction = NewObject<UWeaponAction>(InSource, WeaponActionClass);
	WeaponAction->SetWeapon(InWeapon);
	if (bInitialize)
		WeaponAction->InitializeAction(InSource);

	return WeaponAction;
}

void UWeaponAction::InitializeAction_Implementation(UGameplayActionComponent* InSource)
{
	Super::InitializeAction_Implementation(InSource);

	for (FEffectConfigurationData& EffectConfigData : ActionEffects)
	{
		//if the effect targets health and not self, it should be multiplied by the weapon rating
		if (EffectConfigData.Attribute == FActionSystemTags::Get().Attr_Health)
		{
			EffectConfigData.EffectMagnitude *= Weapon->GetDamageRating();
			EffectConfigData.EffectMinMagnitude *= Weapon->GetDamageRating();
			EffectConfigData.EffectMaxMagnitude *= Weapon->GetDamageRating();
		}
	}

}
