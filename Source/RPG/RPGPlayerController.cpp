// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "RPGCharacter.h"
#include "Engine/World.h"
#include "UI/UIManagementComponent.h"
#include "Interfaces/PlayerInteraction/Selectable.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RPGHUD.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/Actions/Action_Move.h"
#include "Interfaces/PlayerInteraction/Interactable.h"
#include "InventorySystem/InventoryComponent.h"

#pragma region RPCAccessors
void ARPGPlayerController::EndTurnForCharacter_Implementation(UGameplayActionComponent* InGameplayActionComponent)
{
	//ADD LOGIC TO CHECK IF THIS Controller is allowed do this
	ARPGCharacter* InChar = Cast<ARPGCharacter>(InGameplayActionComponent->GetOwner());
	if (InChar->GetCharacterTurn() && InChar->IsActiveTurnCharacter())
	{
		InChar->EndCharacterTurn();
	}
	else if (InGameplayActionComponent->GetTurn())
	{
		//add code to go to the server to end the turn from an action component
	}
	
}

void ARPGPlayerController::Server_EquipSlotForInventoryWithItem_Implementation(UInventoryComponent* InventoryToEquip, UItemEquippable* ItemToEquip, EEquipmentSlot SlotToEquipTo)
{
	InventoryToEquip->EquipSlot(SlotToEquipTo, ItemToEquip);
}


void ARPGPlayerController::Server_UnequipSlotForInventory_Implementation(UInventoryComponent* InventoryToUnequip, EEquipmentSlot SlotToUnequip)
{
	InventoryToUnequip->UnequipSlot(SlotToUnequip);
}

void ARPGPlayerController::Server_ActorInteractWithInteractable_Implementation(AActor* Interactor, const TScriptInterface<IInteractable>& Interactable)
{
	if (!Interactable.GetObject())
	{
		return;
	}

	IInteractable::Execute_Interact(Interactable.GetObject(), Interactor);
}

#pragma endregion RPCAccessors

ARPGPlayerController::ARPGPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	UIManagementComponent = CreateDefaultSubobject<UUIManagementComponent>(TEXT("UIManagementComponent"));
}

void ARPGPlayerController::AddEncounterToUI(AEncounter* Encounter)
{
	UIManagementComponent->AddEncounterUI(Encounter);
}

void ARPGPlayerController::TryPrepareAction(UAction* Action, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	Server_TryPrepareAction(Action, TargetLocation, TargetActor);
}

void ARPGPlayerController::Server_TryPrepareAction_Implementation(UAction* Action, const FVector& TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	Action->TryPrepareAction(TargetLocation, TargetActor);
}

void ARPGPlayerController::TryExecuteAction(UAction* Action, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	Server_TryExecuteAction(Action, TargetLocation, TargetActor);
}

void ARPGPlayerController::Server_TryExecuteAction_Implementation(UAction* Action, const FVector& TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	Action->TryExecuteAction(TargetLocation, TargetActor);
}

void ARPGPlayerController::CancelAction(UAction* Action)
{
	Server_CancelAction(Action);
}

void ARPGPlayerController::Server_CancelAction_Implementation(UAction* Action)
{
	Action->ResetAction();
}

void ARPGPlayerController::Client_PlayActionCue_Implementation(UAction* Action, const FCueConfigurationData& ConfigData, UGameplayActionComponent* TargetComponent, const FVector& TargetLocation)
{
	UActionCueBase::CreateAndPlayCue(ConfigData, Action->GetSource(),TargetComponent, TargetLocation);
}

void ARPGPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	//this will be useful in a bunch of different places, so declare it outside
	//later, can wrap this in an if OR
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, true, Hit);
	

	if(bPrimaryInputPressed)
	{
		//if the input has been pressed for at least half a second, start selection marquee
		if (GetWorld()->GetTimeSeconds() - TimePrimaryInputPressed > 0.1f)
		{
			ARPGHUD* MyRPGHUD = GetHUD<ARPGHUD>();
			if (MyRPGHUD && !MyRPGHUD->IsDrawing())
			{
				MyRPGHUD->StartSelection(this);
			}
		}
	}
	else
	{
		FollowTime = 0.f;
	}

	

	//if the player currently has a character with a gameplay action component selected, check if that character has an active action
	if (Selected.Num() > 0)
	{
		//consider putting this somewhere it can be just done once and then reused throughout the game
		TArray<EActionState> StatesToTick;
		StatesToTick.Add(EActionState::Preparing);
		StatesToTick.Add(EActionState::Executing);
		for (int i = 0; i < Selected.Num(); i++)
		{
			if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
			{
				//this selected character has an action component, check if it has an active action. 
				UGameplayActionComponent* SelectedComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));

				if (!SelectedComponent->GetActiveAction() || !StatesToTick.Contains(SelectedComponent->GetActiveAction()->GetState()))
				{
					continue;
				}
				SelectedComponent->GetActiveAction()->Tick();
			}
		}
	}
}

void ARPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("PrimaryClick", IE_Pressed, this, &ARPGPlayerController::OnPrimaryPressed);
	InputComponent->BindAction("PrimaryClick", IE_Released, this, &ARPGPlayerController::OnPrimaryReleased);
	InputComponent->BindAction("SecondaryClick", IE_Pressed, this, &ARPGPlayerController::OnSecondaryPressed);
	InputComponent->BindAction("SecondaryClick", IE_Released, this, &ARPGPlayerController::OnSecondaryReleased);
	InputComponent->BindAction("DeselectAll", IE_Pressed, this, &ARPGPlayerController::Server_DeselectAll);
	InputComponent->BindAction("OpenInventory", IE_Released, this, &ARPGPlayerController::OpenInventory);
	

	//Hotbar Slots
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarOne", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 0);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarTwo", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 1);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarThree", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 2);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarFour", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 3);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarFive", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 4);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarSix", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 5);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarSeven", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 6);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarEight", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 7);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarNine", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 8);
	InputComponent->BindAction<FHotbarInputSwitch>("HotbarTen", IE_Released, this, &ARPGPlayerController::UseHotBarSkill, 9);
}
	
#pragma region ClientInteraction

void ARPGPlayerController::OnPrimaryPressed()
{
	ARPGHUD* MyRPGHUD = GetHUD<ARPGHUD>();
	if (MyRPGHUD)
	{
		MyRPGHUD->EmptyHighlightActors();
	}
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, true, Hit);

	if (Selected.Num() > 0)
	{
		for (int i = 0; i < Selected.Num(); i++)
		{
			//check whether this player controller can command this actor right now
			if (!CanCommandActor(Selected[i]))
			{
				//if it can't, move to next
				continue;
			}
			if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
			{
				//this selected character has an action component, check if it has an active action. 
				UGameplayActionComponent* SelectedComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));

				if (!SelectedComponent->GetActiveAction() || SelectedComponent->GetActiveAction()->GetState() != EActionState::Preparing)
				{
					continue;
				}
				
				//Selected Component has an active action and its state is preparing, try execute
				UAction* ComponentActiveAction = SelectedComponent->GetActiveAction();
				TryExecuteAction(ComponentActiveAction, Hit.Location, Hit.GetActor());
				bPrimaryInputPressed = false;
				return;
			}
			
		}
		//if we're not using an action, check if we can interact with the box underneath the cursor
		if (Hit.GetActor())
		{
			AActor* ActorUnderCursor = Hit.GetActor();
			if (Cast<IInteractable>(ActorUnderCursor))
			{
				//replicated interfaces must be passed as const references, this is why we're not just using IInteractable*
				const TScriptInterface<IInteractable>& Interactable = TScriptInterface<IInteractable>(ActorUnderCursor);
				Server_ActorInteractWithInteractable(Selected[0], Interactable);
				//IInteractable::Execute_Interact(ActorUnderCursor, Selected[0]);
			}
		}
	}



	//Set Input Variable so we know to make the selection marquee.
	bPrimaryInputPressed = true;
	//Set the time the input was pressed so we can check for the button being held to start a selection marquee
	TimePrimaryInputPressed = GetWorld()->GetTimeSeconds();
	
	
	SelectionStartLocation = Hit.Location;
	UGameplayStatics::ProjectWorldToScreen(this, SelectionStartLocation,ScreenspaceStartLocation, false);
}

void ARPGPlayerController::OnPrimaryReleased()
{
	if (bPrimaryInputPressed == false)
	{
		return;
	}

	bPrimaryInputPressed = false;
	TArray<AActor*> ToSelects;
	
	//When the player releases primary, these are the following cases it should allow for:
	//If currently using an action, check whether the Action can execute on target
		//if it can, execute
		//it if can't, ping a UI notify event
		//return

	//If there is not an action being ran:
		//A: If a selection marquee exists, go to selecting multiple
	
	ARPGHUD* MyRPGHUD = GetHUD<ARPGHUD>();
	if (MyRPGHUD)
	{
		MyRPGHUD->EndSelection(this);
	}
	if (MyRPGHUD && MyRPGHUD->GetHighlightedActors().Num() > 0)
	{
		for (AActor* HighlightedActor : MyRPGHUD->GetHighlightedActors())
		{
			ToSelects.Add(HighlightedActor);
		}

		MyRPGHUD->GetHighlightedActors().Empty();
	}
	else //B: If no selection marquee exists, select beneath cursor
	{
		AActor* ActorBeneathCursor = GetActorBeneathCursor();
		if (ActorBeneathCursor)
		{
			ISelectable* Selectable = Cast<ISelectable>(ActorBeneathCursor);
			if (Selectable)
			{
				//the Actor beneath the cursor is selectable
				ToSelects.Add(ActorBeneathCursor);

			}
		}
	}
		
	


	//If there is anything in the ToSelect Array, select it.
	if (ToSelects.Num() > 0)
	{
		Server_DeselectAll();
		Server_SelectCharacters(ToSelects);
	}

	//Destroy selection Marquee
	
}

void ARPGPlayerController::OnSecondaryPressed()
{
	//if an action is already preparing, we can cancel it
	if (Selected.Num() > 0)
	{
		for (int i = 0; i < Selected.Num(); i++)
		{
			//check whether this player controller can command this actor right now
			if (!CanCommandActor(Selected[i]))
			{
				//if it can't, move to next
				continue;
			}
			if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
			{
				//this selected character has an action component, check if it has an active action. 
				UGameplayActionComponent* SelectedComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));

				if (!SelectedComponent->GetActiveAction() || SelectedComponent->GetActiveAction()->GetState() != EActionState::Preparing)
				{
					continue;
				}

				//Selected Component has an active action and its state is preparing, try execute
				UAction* ComponentActiveAction = SelectedComponent->GetActiveAction();
				CancelAction(ComponentActiveAction);
				bSecondaryInputPressed = false;
				return;
			}
		}
	}
	
	bSecondaryInputPressed = true;
	TimeSecondaryInputPressed = GetWorld()->GetTimeSeconds();

	//this second loop is inefficient - review if I can use the first loop to do this
	if (Selected.Num() > 0)
	{
		for (int i = 0; i < Selected.Num(); i++)
		{
			//check whether this player controller can command this actor right now
			if (!CanCommandActor(Selected[i]))
			{
				//if it can't, move to next
				continue;
			}
			if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
			{
				//this selected character has an action component, check if it has an active action. 
				UGameplayActionComponent* SelectedComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));

				UAction_Move* MoveAction = SelectedComponent->GetMoveAction();
				if (!MoveAction)
				{
					continue;
				}

				Server_TryPrepareAction(MoveAction);
			}
		}
	}
}

void ARPGPlayerController::OnSecondaryReleased()
{
	//if the boolean is false, it means we intentionally blocked this release from doing anything
	if (!bSecondaryInputPressed)
	{
		return;
	}

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	FVector HitLocation = Hit.Location;
	bSecondaryInputPressed = false;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);

	if (Selected.Num() > 0)
	{
		for (int i = 0; i < Selected.Num(); i++)
		{
			if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
			{
				//check whether this player controller can command this actor right now
				if (!CanCommandActor(Selected[i]))
				{
					//if it can't, move to next
					continue;
				}
				//this selected character has an action component, check if it has an active action. 
				UGameplayActionComponent* SelectedComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));

				UAction_Move* MoveAction = SelectedComponent->GetMoveAction();
				if (!MoveAction)
				{
					continue;
				}

				Server_TryExecuteAction(MoveAction, Hit.Location, Hit.GetActor());
			}
		}
	}
}


void ARPGPlayerController::OpenInventory()
{
	//find the first selected character who is both player controlled and has an inventory component and open that inventory
	for (int i = 0; i < Selected.Num(); i++)
	{
		UPlayerControlComponent* ControlComp = Cast<UPlayerControlComponent>(Selected[i]->GetComponentByClass(UPlayerControlComponent::StaticClass()));
		UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(Selected[i]->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (ControlComp && ControlComp->IsPlayerControlled() && InventoryComp)
		{
			UIManagementComponent->OpenInventory(InventoryComp);
			return;
		}
	}
	//if no selected character was found with an inventory, call the nullpointer open method
	UIManagementComponent->OpenInventory();
}

void ARPGPlayerController::UseHotBarSkill(const int32 HotBarSlotIndex)
{
	
		//if we find one, check if it has a action bound to that slot. 
		//TryPrepare that action
	//get the first selected character with an action component.
	for (int i = 0; i < Selected.Num(); i++)
	{
		//check whether this player controller can command this actor right now
		if (!CanCommandActor(Selected[i]))
		{
			//if it can't, move to next
			continue;
		}
		if (Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()))
		{
			UGameplayActionComponent* ActionComponent = Cast<UGameplayActionComponent>(Selected[i]->GetComponentByClass(UGameplayActionComponent::StaticClass()));
			if (ActionComponent->GetBoundActions()[HotBarSlotIndex] == nullptr)
			{
				return;
			}
			TryPrepareAction(ActionComponent->GetBoundActions()[HotBarSlotIndex]);
			return;
		}
	}
}

bool ARPGPlayerController::CanCommandActor(AActor* ActorToCommand)
{
	UPlayerControlComponent* PlayerControlComponent = Cast<UPlayerControlComponent>(ActorToCommand->GetComponentByClass(UPlayerControlComponent::StaticClass()));
	if (!PlayerControlComponent)
	{
		//Actor cannot be commanded by a player at all
		return false;
	}

	if (!PlayerControlComponent->IsPlayerControlled())
	{
		//Actor is not player controlled
		return false;
	}

	//Actor is player controlled, but not by this player, can't command
	if (PlayerControlComponent->GetPlayerController() != this)
	{
		return false;
	}

	//Finally, check for a turn on the actor, and if so check if its currently their turn
	UGameplayActionComponent* ActorActionComp = Cast<UGameplayActionComponent>(ActorToCommand->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (!ActorActionComp)
	{
		//actors without an action comp can't take actions so it makes no sense to order them
		return false;
	}

	if (ActorActionComp->GetTurn())
	{
		//Actor has a turn, check if it's currently their turn
		if (ActorActionComp->GetTurn()->IsCurrentTurn())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//actor is controlled by the player and doesn't have a turn, we can control them right now
		return true;
	}
}

#pragma endregion ClientPlayerInteraction

#pragma region ServerPlayerInteraction
void ARPGPlayerController::Server_SelectCharacters_Implementation(const TArray<AActor*>& ToSelects)
{
	//At present this will just select all the characters
	//Need to have a think about how marquee select should work when at least one character is in an encounter
	//Gut feeling is it should select only the first encounter-relevant character found
	for (AActor* ToSelect : ToSelects)
	{
		/*ARPGCharacter* Char = Cast<ARPGCharacter>(ToSelect);
		if (Char && Char->GetCharacterTurn())
		{
			Server_DeselectAll();
		}*/
		Server_SelectCharacter(ToSelect);
		/*if (Char && Char->GetCharacterTurn())
		{
			break;
		}*/
	}
}


void ARPGPlayerController::Server_SelectCharacter_Implementation(AActor* ToSelect)
{
	if (!Cast<ISelectable>(ToSelect))
	{
		return;
	}
	//ISelectable::Execute_Select(Cast<UObject>(ToSelect), this);
	Selected.Add(ToSelect);
	Client_SelectCharacterCosmetics(ToSelect);
}

void ARPGPlayerController::Server_DeselectAll_Implementation()
{
	while (Selected.Num() > 0)
	{
		Server_DeselectCharacter(Selected[0]);
	}
}

void ARPGPlayerController::Server_DeselectCharacter_Implementation(AActor* ToDeselect)
{
	if (!Cast<ISelectable>(ToDeselect))
	{
		return;
	}
	Selected.Remove(ToDeselect);
	Client_DeselectCharacterCosmetics(ToDeselect);

}

#pragma endregion ServerPlayerInteraction

#pragma region ClientInteractionNotifies
void ARPGPlayerController::Client_SelectCharacterCosmetics_Implementation(AActor* ToSelect)
{
	if (!Cast<ISelectable>(ToSelect))
	{
		return;
	}
	//Execute Select Event on the Interface Holder
	ISelectable::Execute_Select(Cast<UObject>(ToSelect), this);

	//Dispatch Event Notify to UIManager to add selected character UI elements
	UIManagementComponent->ChangeSelectedActor(ToSelect);
}



void ARPGPlayerController::Client_DeselectCharacterCosmetics_Implementation(AActor* ToDeselect)
{
	if (!Cast<ISelectable>(ToDeselect))
	{
		return;
	}
	//Execute Deselect on the Interface Holder
	ISelectable::Execute_Deselect(Cast<UObject>(ToDeselect), this);
	//Dispatch Event Notify to UIManager to remove selected character UI elements
	UIManagementComponent->ChangeSelectedActor(nullptr);
}

#pragma endregion ClientInteractionNotifies

AActor* ARPGPlayerController::GetActorBeneathCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, true, Hit);
	AActor* Target = Hit.GetActor();
	return Target;
}



#pragma region Replication
void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, Selected);
	DOREPLIFETIME(ARPGPlayerController, Controlled);
}

bool ARPGPlayerController::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	return bWroteSomething;
}
#pragma endregion Replication
