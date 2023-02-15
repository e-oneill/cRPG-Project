// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu.h"
#include "Components/CanvasPanel.h"

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	bIsFocusable = true;

}

bool UInGameMenu::IsCurrentTabOfClass(TSubclassOf<UUserWidget> InClass)
{
	if (TabPanel->GetChildrenCount() < 1)
	{
		return false;
	}

	TSubclassOf<UUserWidget> CurrentTabClass = TabPanel->GetChildAt(0)->GetClass();
	
	return CurrentTabClass == InClass;
}
