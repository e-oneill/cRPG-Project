// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_ControlledCharAvatar.generated.h"

class UProgressBar;
class UGameplayActionComponent;
class UActionAttribute;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPG_API UUI_ControlledCharAvatar : public UUserWidget
{
	GENERATED_BODY()

public:
	static UUI_ControlledCharAvatar* CreateControlledCharAvatar(UGameplayActionComponent* InActionComponent, APlayerController* LocalPlayer, TSubclassOf<UUI_ControlledCharAvatar> WidgetClass);

protected:
	void Init(UGameplayActionComponent* InActionComponent);

	void UpdateHealthValues(UActionAttribute* HealthAttribute);

	UFUNCTION(BlueprintNativeEvent)
		void OnHealthChanged(UGameplayActionComponent* InActionComponent, UActionAttribute* InAttr);

	UFUNCTION(BlueprintNativeEvent)
		void OnPhysicalArmourChanged(UGameplayActionComponent* InActionComponent, UActionAttribute* InAttr);

	UFUNCTION(BlueprintNativeEvent)
		void OnMagicalArmourChanged(UGameplayActionComponent* InActionComponent, UActionAttribute* InAttr);
	
	UPROPERTY(BlueprintReadOnly)
	UGameplayActionComponent* ActionComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPhysicalArmour;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPhysicalArmour;


};
