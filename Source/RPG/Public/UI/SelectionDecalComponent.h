// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "SelectionDecalComponent.generated.h"

/**
 * 
 */

 class UMaterialInstance;

UCLASS()
class RPG_API USelectionDecalComponent : public UDecalComponent
{
	GENERATED_BODY()
public:
	USelectionDecalComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		TObjectPtr<class UMaterialInstance> ControlledDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		TObjectPtr<class UMaterialInstance> FriendlyDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		TObjectPtr<class UMaterialInstance> NeutralDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		TObjectPtr<class UMaterialInstance> EnemyDecalMaterial;

public:
	TObjectPtr<class UMaterialInstance> GetFriendlyDecalMaterial() const { return FriendlyDecalMaterial; }
	void SetFriendlyDecalMaterial(TObjectPtr<class UMaterialInstance> val) { FriendlyDecalMaterial = val; }
	TObjectPtr<class UMaterialInstance> GetNeutralDecalMaterial() const { return NeutralDecalMaterial; }
	void SetNeutralDecalMaterial(TObjectPtr<class UMaterialInstance> val) { NeutralDecalMaterial = val; }
	TObjectPtr<class UMaterialInstance> GetControlledDecalMaterial() const { return ControlledDecalMaterial; }
	void SetControlledDecalMaterial(TObjectPtr<class UMaterialInstance> val) { ControlledDecalMaterial = val; }
	TObjectPtr<class UMaterialInstance> GetEnemyDecalMaterial() const { return EnemyDecalMaterial; }
	void SetEnemyDecalMaterial(TObjectPtr<class UMaterialInstance> val) { EnemyDecalMaterial = val; }
};
