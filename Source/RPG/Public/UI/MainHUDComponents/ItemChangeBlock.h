// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemChangeBlock.generated.h"

class UTextBlock;
class UItemBase;

/**
 * 
 */
UCLASS()
class RPG_API UItemChangeBlock : public UUserWidget
{
	GENERATED_BODY()

public:
	static UItemChangeBlock* CreateItemChangeBlock(APlayerController* LocalPlayer, UItemBase* NewItem, UItemBase* CurrentInventoryItem, TSubclassOf<UItemChangeBlock> WidgetClass);
	void Init(UItemBase* NewItem, UItemBase* CurrentInventoryItem);

	UTextBlock* GetItemName() const { return ItemName; }
	void SetItemName(UTextBlock* val) { ItemName = val; }
	UTextBlock* GetNumberChanged() const { return NumberChanged; }
	void SetNumberChanged(UTextBlock* val) { NumberChanged = val; }
	UTextBlock* GetNumberRemaining() const { return NumberRemaining; }
	void SetNumberRemaining(UTextBlock* val) { NumberRemaining = val; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UTextBlock* ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UTextBlock* NumberChanged;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UTextBlock* NumberRemaining;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 2.f;

	UFUNCTION()
	void StartFadeOut();

};
