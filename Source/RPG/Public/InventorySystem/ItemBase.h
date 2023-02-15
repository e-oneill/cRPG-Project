// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "Interfaces/InterfaceVisible.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNonStaticItemEvent);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPG_API UItemBase : public UReplicatedObject, public IInterfaceVisible
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bCanStack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxStackSize;
	UPROPERTY(VisibleInstanceOnly, Replicated, BlueprintReadWrite);
	int32 StackSize = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* ItemIcon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UInventoryComponent* OwningInventory;

public:
	bool CanStack() const { return bCanStack; }
	void CanStack(bool val) { bCanStack = val; }
	int32 GetMaxStackSize() const { return MaxStackSize; }
	void SetMaxStackSize(int32 val) { MaxStackSize = val; }
	int32 GetStackSize() const { return StackSize; }
	void SetStackSize(int32 val) { StackSize = val; OnItemUpdate.Broadcast(); }
	UInventoryComponent* GetOwningInventory() const { return OwningInventory; }
	void SetOwningInventory(UInventoryComponent* val) { OwningInventory = val; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(BlueprintCallable)
	FName GetNameForUI() override
	{
		return ItemName;
	}

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetImageForUI() override
	{
		return ItemIcon;
	}
	//event that should be used to notify when an item's data changes. Only add this to properties that should change at runtime
	UPROPERTY(BlueprintAssignable)
		FNonStaticItemEvent OnItemUpdate;

	UPROPERTY(BlueprintAssignable)
		FNonStaticItemEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable)
		FNonStaticItemEvent OnItemAdded;

	
};
