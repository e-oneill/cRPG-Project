// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUDComponents/ItemChangeBlock.h"
#include "InventorySystem/ItemBase.h"
#include "Components/TextBlock.h"

UItemChangeBlock* UItemChangeBlock::CreateItemChangeBlock(APlayerController* LocalPlayer, UItemBase* NewItem, UItemBase* CurrentInventoryItem, TSubclassOf<UItemChangeBlock> WidgetClass)
{
	FName WidgetName = FName((TEXT("%s_ItemChange"), CurrentInventoryItem->GetName()));
	UItemChangeBlock* NewWidget = CreateWidget<UItemChangeBlock>(LocalPlayer, WidgetClass, WidgetName);
	if (!NewWidget)
	{
		return nullptr;
	}

	NewWidget->Init(NewItem, CurrentInventoryItem);

	return NewWidget;
}

void UItemChangeBlock::Init(UItemBase* NewItem, UItemBase* CurrentInventoryItem)
{
	FString NameAsString = NewItem->GetNameForUI().ToString();
	ItemName->SetText(FText::FromString(NameAsString));

	int StackSizeChange = NewItem->GetStackSize();
	NumberChanged->SetText(FText::FromString(FString::FromInt(StackSizeChange)));

	int CurrentStackSize = CurrentInventoryItem->GetStackSize();
	if (CurrentStackSize != 0)
	{
		NumberRemaining->SetText(FText::FromString(FString::FromInt(CurrentStackSize)));
	}
	FTimerHandle FadeOutTimer;
	GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(FadeOutTimer, this, &UItemChangeBlock::StartFadeOut, Duration);

}

void UItemChangeBlock::StartFadeOut()
{
	RemoveFromViewport();
}
