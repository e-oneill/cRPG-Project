// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManagementComponent.h"
#include "UI/MainHUD.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "UI/InGameMenu.h"
#include "UI/InventoryUI.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values for this component's properties
UUIManagementComponent::UUIManagementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUIManagementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocallyControlled())
	{
		return;
	}
	
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	HUD = CreateWidget<UMainHUD>(PC, HUDClass, TEXT("Main HUD"));
	if (HUD)
	{
		HUD->AddToViewport(0);
		FocusWidget = HUD;
	}

	InGameMenu = CreateWidget<UInGameMenu>(PC, InGameMenuClass, TEXT("InGameMenu"));
	
	//create the ingame menu, we can make it visible later
	
	
	// ...
	
}


bool UUIManagementComponent::IsLocallyControlled()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	return (PC && PC->IsLocalController());
}

// Called every frame
void UUIManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUIManagementComponent::AddEncounterUI(AEncounter* Encounter)
{
	HUD->AddTurnTrackersToScreen(Encounter);
}

void UUIManagementComponent::OpenGameMenuUI()
{
	if (!InGameMenu || !IsLocallyControlled())
	{
		return;
	}
	HUD->SetVisibility(ESlateVisibility::Collapsed);
	if (!InGameMenu->IsInViewport())
	{
		InGameMenu->AddToViewport(10);
	}
	

	InGameMenu->SetVisibility(ESlateVisibility::Visible);
	//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwner<APlayerController>(), InGameMenu, EMouseLockMode::LockInFullscreen);
	FocusWidget=InGameMenu;
}

void UUIManagementComponent::CloseGameMenuUI()
{
	InGameMenu->SetVisibility(ESlateVisibility::Collapsed);
	HUD->SetVisibility(ESlateVisibility::Visible);

	//UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwner<APlayerController>(), HUD, EMouseLockMode::LockInFullscreen);
	FocusWidget = HUD;
}

void UUIManagementComponent::OpenInventory(UInventoryComponent* InventoryToOpen /*= nullptr*/)
{
	//if the inventory is currently open, close this UI.
	if (FocusWidget == InGameMenu && InGameMenu->IsCurrentTabOfClass(InGameMenu->GetInventoryUIClass()))
	{
		CloseGameMenuUI();
		return;
	}

	//if the in game menu is not open, open it.
	if (FocusWidget != InGameMenu)
	{
		OpenGameMenuUI();
	}

	UCanvasPanel* MenuTabPanel = InGameMenu->GetTabPanel();
	if (MenuTabPanel)
	{
		MenuTabPanel->ClearChildren();
		APlayerController* PC = Cast<APlayerController>(GetOwner()); 
		MenuTabPanel->AddChildToCanvas(UInventoryUI::CreateInventoryUI(PC, InGameMenu->GetInventoryUIClass(), InventoryToOpen));
		TArray<UPanelSlot*> PanelSlots = MenuTabPanel->GetSlots();
		UCanvasPanelSlot* TabSlot = Cast<UCanvasPanelSlot>(PanelSlots[0]);
		FAnchors Anchors = FAnchors(0.f, 0.f, 1.f, 1.f);
		
		TabSlot->SetAnchors(Anchors);
		TabSlot->SetOffsets(0.f);
		
	}

	//if the in game menu is already open, switch tab to inventory

	

	


}

void UUIManagementComponent::ChangeSelectedActor(AActor* Actor)
{
	SelectedActor = Actor;

	HUD->HandleSelectedActorChange(SelectedActor);
	OnSelectedChanged.Broadcast(Actor);
}

