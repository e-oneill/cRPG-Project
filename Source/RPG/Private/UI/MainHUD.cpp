// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"
#include "../RPG.h"
#include "GameState/Encounter.h"
#include "GameState/Turn.h"
#include "UI/EncounterTracker.h"
#include "Components/HorizontalBox.h"
#include "UI/MainHUDComponents/HotBarButton.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "ActionSystemTags.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/RPGGameState.h"
#include "../RPGPlayerController.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "UI/MainHUDComponents/UI_ControlledCharAvatar.h"
#include "Components/VerticalBox.h"
#include "InventorySystem/InventoryComponent.h"
#include "UI/MainHUDComponents/ItemChangeBlock.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (HotBarContainer->GetChildrenCount() != 10)
	{
		UE_LOG(LogRPG, Error, TEXT("Not enough slots on hotbar"));
	}
	//initialise selected component
	SelectedComponent = nullptr;
	//bind to end turn button
	EndTurnButton->OnPressed.AddUniqueDynamic(this, &UMainHUD::OnPressEndTurn);

	ARPGGameState* GameState = Cast<ARPGGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		TArray<UPlayerControlComponent*> PlayerControlComps = GameState->GetControlledActors();
		for (int i = 0; i < PlayerControlComps.Num(); i++)
		{
			UPlayerControlComponent* PlayerControlComp = PlayerControlComps[i];
			if (!PlayerControlComp->IsPlayerControlled())
			{
				continue;
			}
			AActor* ControlledActor = PlayerControlComp->GetOwner();
			UGameplayActionComponent* ControlledGAC = Cast<UGameplayActionComponent>(ControlledActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
			UUI_ControlledCharAvatar* NewAvatar = UUI_ControlledCharAvatar::CreateControlledCharAvatar(ControlledGAC, GetOwningPlayer(), CharAvatarClass);
			if (NewAvatar)
			{
				UE_LOG(LogTemp, Warning, TEXT("Creating UI char avatar"));
				CharacterAvatarsContainer->AddChild(NewAvatar);
			}
		}
	}

}

void UMainHUD::OnTurnAssigned_Implementation(UTurn* InTurn)
{
	Turn = InTurn;
	//UE_LOG(LogTemp, Log, TEXT("Calling HUD turn assignment callback"));
	if (Turn)
	{
		Turn->OnTurnStart.AddUniqueDynamic(this, &UMainHUD::OnTurnAssigned);
		Turn->OnTurnEnd.AddUniqueDynamic(this, &UMainHUD::OnTurnEnd);
		//Turn is being assigned

		//Get the movement attribute and bind to it
		UActionAttribute* MovementAttr = SelectedComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_Movement);
		if (MovementAttr)
			MovementAttr->OnAttributeChanged.AddUniqueDynamic(this, &UMainHUD::OnMovementChanged);

		EndTurnButton->SetVisibility(ESlateVisibility::Visible);
		if (Turn->IsCurrentTurn() && Turn->GetActionComponent() == SelectedComponent)
		{
			EndTurnButton->SetIsEnabled(true);
		}
		else
		{
			OnTurnEnd(Turn);
		}
	}
	else
	{
		EndTurnButton->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UMainHUD::OnTurnEnd_Implementation(UTurn* InTurn)
{
	EndTurnButton->SetIsEnabled(false);
}

void UMainHUD::OnMovementChanged_Implementation(UGameplayActionComponent* InActionComponent, UActionAttribute* InMovementAttr)
{
	UE_LOG(LogTemp, Log, TEXT("Tracking movement change..."));
}

void UMainHUD::OnAPChanged_Implementation(UGameplayActionComponent* InActionComponent, UActionAttribute* InMovementAttr)
{
	UE_LOG(LogTemp, Log, TEXT("Tracking AP change..."));
}

void UMainHUD::OnPressEndTurn()
{
	if (!Turn || !Turn->IsCurrentTurn())
	{
		//End Turn button should not be visible when Turn has not been set and if this is not the current turn, don't end it
		return;
	}

	ARPGPlayerController* PlayerController = GetOwningPlayer<ARPGPlayerController>();
	if (!PlayerController)
	{
		//Can't end the turn if not using the right controller
		return;
	}

	PlayerController->EndTurnForCharacter(SelectedComponent);


}

void UMainHUD::SetupActionPoints(UGameplayActionComponent* ActionComponent, UActionAttribute* APAttribute)
{
	if (!ActionComponent)
	{
		return;
	}

	OnAPChanged(ActionComponent, APAttribute);
}

void UMainHUD::HandleSelectedActorChange(AActor* InSelectedActor)
{
	//TODO: Needs new logic to behave differently if the selected actor is not controlled by the player
	
	//start by setting up bindings to events that will drive UI changes.
	UpdateSelectedActorEventBindings(InSelectedActor);

	if (InSelectedActor)
	{
		UPlayerControlComponent* PlayerControlComp = Cast<UPlayerControlComponent>(InSelectedActor->GetComponentByClass(UPlayerControlComponent::StaticClass()));
		//A: There is no player control comp or the actor is not player controlled
		if (!PlayerControlComp || !PlayerControlComp->IsPlayerControlled())
		{
			bControlledByPlayer = false;
		}
		//B: There is a player control comp but this is not the controlling player
		else if (PlayerControlComp->GetPlayerController() != GetOwningPlayer())
		{
			bControlledByPlayer = false;
		}
		//C: This must be the controlling player
		else
		{
			bControlledByPlayer = true;
		}

		UInventoryComponent* InInventoryComponent = Cast<UInventoryComponent>(InSelectedActor->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (InInventoryComponent)
		{
			InventoryComponent = InInventoryComponent;
			InventoryComponent->OnItemAdded.AddUniqueDynamic(this, &UMainHUD::OnItemAdded);
			InventoryComponent->OnItemRemoved.AddUniqueDynamic(this, &UMainHUD::OnItemRemoved);
		}
	}
	else
	{
		bControlledByPlayer = false;
	}
	

	//fill the hotbar with this character's actions
	UGameplayActionComponent* ActionComponent = nullptr;
	SelectedActor = InSelectedActor;
	if (SelectedActor)
	{
		ActionComponent = Cast<UGameplayActionComponent>(SelectedActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	}
	SelectedComponent = ActionComponent;

	//Setup the 
	
	//Selected Actor has an action component
	if (SelectedComponent)
	{
		//bind to join and leave encounter events (do this first, use same implementation for if turn already exists)
		SelectedComponent->OnTurnAssigned.AddUniqueDynamic(this, &UMainHUD::OnTurnAssigned);
		SelectedComponent->OnTurnUnassigned.AddUniqueDynamic(this, &UMainHUD::OnTurnAssigned);
		
		//We call OnTurnAssigned. If there is currently no turn assigned, this will call with nullptr, clearing any existing Turn elements from the UI
		OnTurnAssigned(SelectedComponent->GetTurn());


		//ActionPoints can update outside of combat, so we bind here instead of inside the OnTurnAssigned method
		UActionAttribute* APAttribute = SelectedComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_ActionPoints);
		if (APAttribute)
		{
			SetupActionPoints(SelectedComponent, APAttribute);
			APAttribute->OnAttributeChanged.AddUniqueDynamic(this, &UMainHUD::OnAPChanged);
		}
			

		//assigns actions bound on the action component to the hotbar
		AssignBoundActionsToHotbar(SelectedComponent);
	}
	
	

}

void UMainHUD::AssignBoundActionsToHotbar(UGameplayActionComponent* ActionComponent)
{
	for (int i = 0; i < 10; i++)
	{
		if (bControlledByPlayer && ActionComponent && ActionComponent->GetBoundActions()[i])
		{
			UAction* SlotAction = ActionComponent->GetBoundActions()[i];
			AssignActionToHotBar(SlotAction, i);
		}
		else
		{
			AssignActionToHotBar(nullptr, i);
		}
	}
}

void UMainHUD::UpdateSelectedActorEventBindings(AActor* InSelectedActor)
{
	
	//if SelectedActor is already set and has an action component, remove any bindings
	if (SelectedActor)
	{
		UGameplayActionComponent* OldActionComponent = Cast<UGameplayActionComponent>(SelectedActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
		//Selected Actor has an action component
		if (OldActionComponent)
		{
			OldActionComponent->OnActionBound.RemoveAll(this);
		}
	}
	
	if (InSelectedActor)
	{
		SelectedActor = InSelectedActor;
		UGameplayActionComponent* ActionComponent = Cast<UGameplayActionComponent>(SelectedActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
		//Selected Actor has an action component
		if (ActionComponent)
		{
			if (ActionComponent)
			{
				ActionComponent->OnActionBound.AddUniqueDynamic(this, &UMainHUD::AssignActionToHotBar);
			}
		}
	}
	
}

void UMainHUD::AddTurnTrackersToScreen(AEncounter* ActiveEncounter)
{

	EncounterTracker->SetupEncounter(ActiveEncounter);
}

void UMainHUD::AssignActionToHotBar(UAction* Action, int32 HotBarSlotIndex)
{

	if (HotBarSlotIndex < 0 || HotBarSlotIndex > 9)
	{
		//invalid hot bar slot provided
		return;
	}
	UHotBarButton* HotBarSlot = Cast<UHotBarButton>(HotBarContainer->GetChildAt(HotBarSlotIndex));
	
	HotBarSlot->SetBoundAction(TWeakObjectPtr<UAction>(Action));
}

void UMainHUD::OnItemAdded(UInventoryComponent* Inventory, UItemBase* NewItem, UItemBase* CurrentInventoryItem)
{
	/*UItemChangeBlock* ItemPopup = UItemChangeBlock::CreateItemChangeBlock(GetOwningPlayer(), NewItem, CurrentInventoryItem, ItemPopupClass);
	if (!ItemPopup)
	{
		return;
	}

	ItemPopupsContainer->AddChildToVerticalBox(ItemPopup);*/
}

void UMainHUD::OnItemRemoved(UInventoryComponent* Inventory, UItemBase* NewItem, UItemBase* CurrentInventoryItem)
{

}

int32 UMainHUD::GetNextFreeActionSlot()
{
	for (int i = 0; i < 10; i++)
	{
		UHotBarButton* BarSlot = Cast<UHotBarButton>(HotBarContainer->GetChildAt(i));
		if (!BarSlot->GetBoundAction().IsValid())
		{
			return i;
		}
	}
	//no slots are free, send back minus 1;
	return -1;
}
