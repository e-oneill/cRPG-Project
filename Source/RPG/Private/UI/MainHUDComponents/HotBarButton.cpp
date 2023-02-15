// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUDComponents/HotBarButton.h"
#include "Components/Button.h"
#include "UI/MainHUD.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "../RPGPlayerController.h"

void UHotBarButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	MainHUD = Cast<UMainHUD>(GetRootWidget());

}

void UHotBarButton::SetBoundAction(TWeakObjectPtr<UAction> val)
{
//if val is nullptr, the button is being unbound.
	if (val == nullptr)
	{
		BoundAction = nullptr;
		HotBarButton->SetVisibility(ESlateVisibility::Collapsed);
		
	}
	else
	{
		BoundAction = val;
		HotBarButton->SetVisibility(ESlateVisibility::Visible);
		HotBarButton->OnClicked.AddUniqueDynamic(this, &UHotBarButton::TryPrepareBoundAction);
		FButtonStyle ButtonStyle = HotBarButton->WidgetStyle;
		FSlateBrush NormalStyle = ButtonStyle.Normal;
		UAction* PointerAction = BoundAction.Get();
		NormalStyle.SetResourceObject(PointerAction->GetActionIcon());

		PointerAction->OnActionPrepare.AddUniqueDynamic(this, &UHotBarButton::OnActionStateChange);
		PointerAction->OnActionExecute.AddUniqueDynamic(this, &UHotBarButton::OnActionStateChange);
		PointerAction->OnActionComplete.AddUniqueDynamic(this, &UHotBarButton::OnActionStateChange);
		PointerAction->OnActionCancel.AddUniqueDynamic(this, &UHotBarButton::OnActionStateChange);
	}

	OnBoundActionChanged();

}

void UHotBarButton::TryPrepareBoundAction()
{
	if (BoundAction.IsValid())
	{
		//the Bound Action is valid, so we can fire it
		UAction* Action = BoundAction.Get();
		ARPGPlayerController* OwningRPGPlayer = GetOwningPlayer<ARPGPlayerController>();
		if (OwningRPGPlayer)
		{
			OwningRPGPlayer->TryPrepareAction(Action);
		}

		Action->TryPrepareAction();
	}
	else
	{
		//Bound action is invalid, so unset it
		BoundAction = nullptr;
	}
}
