// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuCharacterSwitch.generated.h"

class UImage;
class UGameplayActionComponent;
class UInventoryComponent;
class APlayerController;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCharacterSwitchEvent, UGameMenuCharacterSwitch*, Switch, AActor*, SwitchCharacter, UGameplayActionComponent*, SwitchActionComponent, UInventoryComponent*, SwitchInventoryComponent);

/**
 * This is a reusable UI component that is used to add a switch functionality to allow a UI screen to swap between characters
 * To use this class, add a widget to a UI screen and then bind to the FCharacterSwitchEvent with a function on that UI that handles the switch
 */
UCLASS()
class RPG_API UGameMenuCharacterSwitch : public UUserWidget
{
	GENERATED_BODY()

public:
	static UGameMenuCharacterSwitch* CreateCharacterSwitchWidget(TSubclassOf<UGameMenuCharacterSwitch> SwitchClass, APlayerController* PlayerController, AActor* Character, UGameplayActionComponent* SwitchActionComponent = nullptr, UInventoryComponent* SwitchInventoryComponent = nullptr);


	AActor* GetCharacter() const { return Character; }
	void SetCharacter(AActor* val) { Character = val; }

	UGameplayActionComponent* GetCharacterActionComponent() const { return CharacterActionComponent; }
	void SetCharacterActionComponent(UGameplayActionComponent* val) { CharacterActionComponent = val; }


	UInventoryComponent* GetCharacterInventoryComponent() const { return CharacterInventoryComponent; }
	void SetCharacterInventoryComponent(UInventoryComponent* val) { CharacterInventoryComponent = val; }
	bool IsActive() const { return bIsActive; }
	void IsActive(bool val) { bIsActive = val; OnActiveChange(); }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UImage* CharacterPortrait;

	AActor* Character;

	UGameplayActionComponent* CharacterActionComponent;

	UInventoryComponent* CharacterInventoryComponent;

	UPROPERTY(BlueprintReadOnly)
	bool bIsActive;


	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnActiveChange();

public:
	UPROPERTY(BlueprintAssignable)
	FCharacterSwitchEvent OnCharacterSwitchActivated;

	UPROPERTY(BlueprintAssignable)
	FCharacterSwitchEvent OnCharacterSwitchContext;
};
