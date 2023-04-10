// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/Action.h"
#include "WeaponAction.generated.h"

class UItemEquippableWeapon;

/**
 * 
 */
UCLASS()
class RPG_API UWeaponAction : public UAction
{
	GENERATED_BODY()
	
public:
	static UWeaponAction* CreateWeaponAction(UItemEquippableWeapon* InWeapon,TSubclassOf<UWeaponAction> WeaponActionClass, UGameplayActionComponent* InSource, bool bInitialize = false);

	void InitializeAction_Implementation(UGameplayActionComponent* InSource) override;

	UItemEquippableWeapon* GetWeapon() const { return Weapon; }
	void SetWeapon(UItemEquippableWeapon* val) { Weapon = val; }
protected:
	UItemEquippableWeapon* Weapon;
};
