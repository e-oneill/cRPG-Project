// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryUI.h"
#include "../RPGPlayerController.h"
#include "../RPG.h"
#include "UI/InGameMenuComponents/InventoryComponents/EquipmentSlotWidget.h"
#include "InventorySystem/InventoryComponent.h"
#include "InventorySystem/InventoryTypes.h"
#include "InventorySystem/ItemEquippable.h"
#include "UI/InGameMenuComponents/InventoryComponents/ItemWidgetBase.h"
#include "Components/WrapBox.h"
#include "GameFramework/RPGGameState.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "UI/InGameMenuComponents/GameMenuCharacterSwitch.h"
#include "Components/VerticalBox.h"

UInventoryUI* UInventoryUI::CreateInventoryUI(APlayerController* PlayerController, TSubclassOf<UInventoryUI> InventoryWidgetClass, UInventoryComponent* InInventory)
{
	UInventoryUI* NewInventoryUI = CreateWidget<UInventoryUI>(PlayerController, InventoryWidgetClass, TEXT("InventoryUI"));
	if (!NewInventoryUI)
	{
		return nullptr;
	}

	NewInventoryUI->Init(InInventory);

	return NewInventoryUI;
}

void UInventoryUI::SetupCharacterSwitchWidgets(ARPGPlayerController* RPGPlayerController)
{
	ARPGGameState* GameState = RPGPlayerController->GetWorld()->GetGameState<ARPGGameState>();
	if (!GameState)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to create InventoryUI but the game state is not a subclass of ARPGGameState."));
		return;
	}
	TArray<UPlayerControlComponent*> PlayerControlledChars = GameState->GetControlledActors();
	ControlledCharactersContainer->ClearChildren();
	for (UPlayerControlComponent* PlayerControlledChar : PlayerControlledChars)
	{

		UInventoryComponent* CharInventoryComponent = Cast<UInventoryComponent>(PlayerControlledChar->GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
		UGameplayActionComponent* CharActionComponent = Cast<UGameplayActionComponent>(PlayerControlledChar->GetOwner()->GetComponentByClass(UGameplayActionComponent::StaticClass()));

		UGameMenuCharacterSwitch* CharacterSwitch = UGameMenuCharacterSwitch::CreateCharacterSwitchWidget(SwitchClass, GetOwningPlayer(), PlayerControlledChar->GetOwner(), CharActionComponent, CharInventoryComponent);
		if (CharInventoryComponent == ActiveInventory)
		{
			CharacterSwitch->IsActive(true);
		}

		CharacterSwitch->OnCharacterSwitchActivated.AddUniqueDynamic(this, &UInventoryUI::HandleCharacterSwitch);
		ControlledCharactersContainer->AddChildToVerticalBox(CharacterSwitch);
	}
}

//TODO: This is a monolithic function, should break it out into smaller methods that each serve a single purpose

void UInventoryUI::Init_Implementation(UInventoryComponent* InventoryComponent)
{
	ActiveInventory = InventoryComponent;

	ARPGPlayerController* RPGPlayerController = Cast<ARPGPlayerController>(GetOwningPlayer());
	if (!RPGPlayerController)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to create InventoryUI but the owning player is not a subclass of ARPGPlayerController."));
		return;
	}


	//get the controlled characters
	SetupCharacterSwitchWidgets(RPGPlayerController);
	
	

	//initialise inventory slots up the value set as the default
	SetupInventorySlots();


	//setup the equipment slot section of the UI
	SetupEquipmentSlots();

}

void UInventoryUI::PostInitProperties()
{
	Super::PostInitProperties();
}

bool UInventoryUI::Initialize()
{
	bool Init = Super::Initialize();

	SlotWidgets.AddUnique(MainHandSlot);
	SlotWidgets.AddUnique(OffHandSlot);
	SlotWidgets.AddUnique(HeadSlot);
	SlotWidgets.AddUnique(NeckSlot);
	SlotWidgets.AddUnique(BootSlot);
	SlotWidgets.AddUnique(BodySlot);
	SlotWidgets.AddUnique(GlovesSlot);
	SlotWidgets.AddUnique(BeltSlot);
	SlotWidgets.AddUnique(TrinketSlot);
	
	return Init;
}

void UInventoryUI::SetupEquipmentSlots()
{
	if (!ActiveInventory)
	{
		//if no inventory comp has been set, we can't do this
		UE_LOG(LogRPG, Error, TEXT("Tried to setup an inventory ui without any active inventory"));
		return;
	}
	//setup initial values of each slot
	for (int i = 0; i < SlotWidgets.Num(); i++)
	{
		SetupEquipmentSlot(SlotWidgets[i]);
	}


	ActiveInventory->OnEquipmentChanged.AddUniqueDynamic(this, &UInventoryUI::OnEquipmentChanged);

}

void UInventoryUI::SetupEquipmentSlot(UEquipmentSlotWidget* SlotToSetup)
{
	//get the equipment slot struct from the inventory
	FEquippedSlot EquippedSlot = ActiveInventory->GetEquippedSlotFromSlotEnum(SlotToSetup->GetEquipmentSlot());
	SlotToSetup->SetEquipmentItem(EquippedSlot.Item);
	//call the callback function that will 
	//OnEquipmentChanged(ActiveInventory, EquippedSlot, EquippedSlot.Item, nullptr);
}

void UInventoryUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryUI::OnEquipmentChanged(UInventoryComponent* Inventory, const FEquippedSlot& EquipmentSlot, UItemEquippable* NewItem, UItemEquippable* OldItem)
{
	UEquipmentSlotWidget* AffectedSlot = GetEquipmentSlotWidgetFromSlot(EquipmentSlot.Slot);
	if (!AffectedSlot)
	{
		//there is no widget for this slot.
		return;
	}

	AffectedSlot->SetEquipmentItem(NewItem);
}

void UInventoryUI::DeactivateCurrentSwitch()
{
	
	for (UWidget* SwitchWidget : ControlledCharactersContainer->GetAllChildren())
	{
		UGameMenuCharacterSwitch* Switch = Cast<UGameMenuCharacterSwitch>(SwitchWidget);
		if (!Switch)
		{
			continue;
		}

		if (Switch->IsActive())
		{
			Switch->IsActive(false);
			break;
		}
	}
}

UEquipmentSlotWidget* UInventoryUI::GetEquipmentSlotWidgetFromSlot(EEquipmentSlot InSlot)
{
	for (UEquipmentSlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget->GetEquipmentSlot() == InSlot)
		{
			return SlotWidget;
		}
	}

	return nullptr;
}

void UInventoryUI::HandleCharacterSwitch(UGameMenuCharacterSwitch* Switch, AActor* SwitchCharacter, UGameplayActionComponent* SwitchActionComponent, UInventoryComponent* SwitchInventoryComponent)
{
	DeactivateCurrentSwitch();
	Switch->IsActive(true);
	Init(SwitchInventoryComponent);
}

void UInventoryUI::SetupInventorySlots()
{
	for (int i = 0; i < DefaultInventorySlots; i++)
	{
		UItemBase* InvItem = nullptr;
		//check if the active inventory has an item at this entry in its array
		if (ActiveInventory && ActiveInventory->GetInventory().Num() > i)
		{
			InvItem = ActiveInventory->GetInventory()[i];
		}

		//if we don't already have a widget for this slot, create one and add it to the wrap box
		if (InventorySlotContainer->GetChildrenCount() < i + 1)
		{
			UItemWidgetBase* InventorySlot = UItemWidgetBase::CreateItemWidget(InventorySlotWidget, GetOwningPlayer(), InvItem, this);
			InventorySlotContainer->AddChild(InventorySlot);
		}
		//if the inventory slot already exists, call init
		else
		{
			UItemWidgetBase* InventorySlot = Cast<UItemWidgetBase>(InventorySlotContainer->GetChildAt(i));
			InventorySlot->Init(InvItem, this);
		}
	}
}
