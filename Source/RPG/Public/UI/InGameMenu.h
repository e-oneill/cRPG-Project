// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

class UCanvasPanel;
class UInventoryUI;
/**
 * 
 */
UCLASS()
class RPG_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = "true"))
	UCanvasPanel* TabPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryUI> InventoryUIClass;
	
	void NativeConstruct() override;

public:
	UCanvasPanel* GetTabPanel() const { return TabPanel; }
	void SetTabPanel(UCanvasPanel* val) { TabPanel = val; }
	TSubclassOf<UInventoryUI> GetInventoryUIClass() const { return InventoryUIClass; }
	void SetInventoryUIClass(TSubclassOf<UInventoryUI> val) { InventoryUIClass = val; }

	bool IsCurrentTabOfClass(TSubclassOf<UUserWidget> InClass);


	
};
