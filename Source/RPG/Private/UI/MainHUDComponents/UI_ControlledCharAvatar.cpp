// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUDComponents/UI_ControlledCharAvatar.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "ActionSystemTags.h"
#include "Components/ProgressBar.h"

UUI_ControlledCharAvatar* UUI_ControlledCharAvatar::CreateControlledCharAvatar(UGameplayActionComponent* InActionComponent, APlayerController* LocalPlayer, TSubclassOf<UUI_ControlledCharAvatar> WidgetClass)
{
	FName WidgetName = FName((TEXT("%s_CharAvatar"), InActionComponent->GetOwner()->GetName()));
	UUI_ControlledCharAvatar* NewWidget = CreateWidget<UUI_ControlledCharAvatar>(LocalPlayer, WidgetClass, WidgetName);
	if (!NewWidget)
	{
		return nullptr;
	}

	NewWidget->Init(InActionComponent);

	return NewWidget;
}

void UUI_ControlledCharAvatar::Init(UGameplayActionComponent* InActionComponent)
{
	ActionComponent = InActionComponent;
	UActionAttribute* HealthAttribute = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_Health);
	if (!HealthAttribute)
	{
		return;
	}

	HealthAttribute->OnAttributeChanged.AddUniqueDynamic(this, &UUI_ControlledCharAvatar::OnHealthChanged);
	
	UpdateHealthValues(HealthAttribute);

}

void UUI_ControlledCharAvatar::UpdateHealthValues(UActionAttribute* HealthAttribute)
{
	CurrentHealth = HealthAttribute->GetAttributeValue();
	MaxHealth = HealthAttribute->GetAttributeBaseValue();
	HealthBar->SetPercent((float)CurrentHealth / MaxHealth);
}

void UUI_ControlledCharAvatar::OnHealthChanged_Implementation(UGameplayActionComponent* InActionComponent, UActionAttribute* InHealthAttr)
{
	UpdateHealthValues(InHealthAttr);
}
