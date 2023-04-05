// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/ItemBase.h"
#include "ItemConsumable.generated.h"

class UAction;

/**
 * 
 */
UCLASS()
class RPG_API UItemConsumable : public UItemBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Uses = 1;

	int32 InitialUses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAction> ConsumableActionType;



	UPROPERTY(BlueprintReadOnly)
	UAction* ConsumableAction;

public:
	UFUNCTION(BlueprintNativeEvent)
	void UseConsumable(AActor* TargetActor = nullptr, FVector TargetLocation = FVector::ZeroVector);

	void PostInitProperties() override;


	void InitializeItem(const FInventoryItemData& ItemData) override;

};
