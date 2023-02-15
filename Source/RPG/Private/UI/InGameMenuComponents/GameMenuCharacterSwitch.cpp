// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenuComponents/GameMenuCharacterSwitch.h"
#include "../RPG.h"
#include "Kismet/KismetInputLibrary.h"

UGameMenuCharacterSwitch* UGameMenuCharacterSwitch::CreateCharacterSwitchWidget(TSubclassOf<UGameMenuCharacterSwitch> SwitchClass, APlayerController* PlayerController, AActor* Character, UGameplayActionComponent* SwitchActionComponent /*= nullptr*/, UInventoryComponent* SwitchInventoryComponent /*= nullptr*/)
{
	if (!SwitchClass)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to create a character switch widget without declaring a blueprint class when constructing"));
		return nullptr;
	}
	FName WidgetName = FName(TEXT("CharSwitch_" + Character->GetName()));
	UGameMenuCharacterSwitch* NewSwitch = CreateWidget<UGameMenuCharacterSwitch>(PlayerController, SwitchClass, WidgetName);
	if (!NewSwitch)
	{
		return nullptr;
	}

	NewSwitch->SetCharacter(Character);
	NewSwitch->SetCharacterActionComponent(SwitchActionComponent);
	NewSwitch->SetCharacterInventoryComponent(SwitchInventoryComponent);

	return NewSwitch;
}

FReply UGameMenuCharacterSwitch::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
 	FKey MouseButtonPressed = UKismetInputLibrary::PointerEvent_GetEffectingButton(InMouseEvent);
	if (MouseButtonPressed == FKey("LeftMouseButton"))
	{
		OnCharacterSwitchActivated.Broadcast(this, Character, CharacterActionComponent, CharacterInventoryComponent);
	}
	else if (MouseButtonPressed == FKey("RightMouseButton"))
	{
		OnCharacterSwitchContext.Broadcast(this, Character, CharacterActionComponent, CharacterInventoryComponent);
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UGameMenuCharacterSwitch::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

