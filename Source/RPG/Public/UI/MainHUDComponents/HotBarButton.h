// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotBarButton.generated.h"

class UButton;
class UAction;
class UMainHUD;

/**
 * 
 */
UCLASS()
class RPG_API UHotBarButton : public UUserWidget
{
	GENERATED_BODY()

	protected:
	TWeakObjectPtr<UMainHUD> MainHUD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UButton* HotBarButton;

	UPROPERTY(BlueprintReadOnly, Category = "Hot Bar")
	TWeakObjectPtr<UAction> BoundAction;

	void NativeConstruct() override;

public:
	TWeakObjectPtr<UAction> GetBoundAction() const { return BoundAction; }
	void SetBoundAction(TWeakObjectPtr<UAction> val); 

	UFUNCTION(BlueprintImplementableEvent)
	void OnBoundActionChanged();

	UFUNCTION()
	void TryPrepareBoundAction();

	UFUNCTION(BlueprintImplementableEvent)
	void OnActionStateChange(UAction* Action, EActionState State, EActionState OldState);

};
