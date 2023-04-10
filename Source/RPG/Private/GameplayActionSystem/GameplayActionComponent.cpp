// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/GameplayActionComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/Actions/Action_Move.h"
#include "../RPG.h"
#include "AI/RPGAIController.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "Engine/ActorChannel.h"
#include "GameplayActionSystem/ActionOwnerInterface.h"
#include "GameplayActionSystem/ActionPlayerControlInterface.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "../RPGPlayerController.h"

// Sets default values for this component's properties
/*UGameplayActionComponent::UGameplayActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}*/


// Called when the game starts
void UGameplayActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner()->HasAuthority())
	{
		for (int i = 0; i < 10; i++)
		{
			BoundActions.Add(nullptr);
		}

		for (TSubclassOf<UAction> ActionClass : DefaultActions)
		{
			GrantAction(ActionClass);
			/*if (ActionClass != UAction_Move::StaticClass())
			{
				BindAction(NewAction, 0);
			}*/
		}
		
		for (FAttributeConfig AttributeConfig : AttributeSet)
		{
			AddAttribute(AttributeConfig);
		}
	}
}


UAction* UGameplayActionComponent::GrantAction(TSubclassOf<UAction> ActionClassToGrant)
{
	UAction* NewAction = NewObject<UAction>(this, ActionClassToGrant);
	GrantAction(NewAction);
	
	return NewAction;
}

UAction* UGameplayActionComponent::GrantAction(UAction* ActionToGrant)
{
	ActionToGrant->InitializeAction(this);
	CharacterActions.Add(ActionToGrant);
	if (ActionToGrant->GetClass() != UAction_Move::StaticClass())
	{
		BindActionToNextFreeSlot(ActionToGrant);
	}
	return ActionToGrant;
	
}

void UGameplayActionComponent::RemoveAction(UAction* ActionToRemove)
{
	//remove bound action
	if (BoundActions.Contains(ActionToRemove))
	{
		for (int i = 0; i < BoundActions.Num(); i++)
		{
			if (BoundActions[i] == ActionToRemove)
			{
				BoundActions[i] = nullptr;
				OnActionBound.Broadcast(nullptr, i);
				break;
			}
		}
	}
	//remove actions from character actions array
	CharacterActions.Remove(ActionToRemove);
}

void UGameplayActionComponent::BindActionToNextFreeSlot(UAction* ActionToBind)
{
	for (int i = 0; i < BoundActions.Num(); i++)
	{
		if (!BoundActions[i])
		{
			return BindAction(ActionToBind, i);
		}
	}
}

void UGameplayActionComponent::BindAction(UAction* ActionToBind, int32 SlotToBindTo)
{
	if (BoundActions.Num() < SlotToBindTo + 1)
	{
		//invalid slot provided, refuse
		return;
	}

	BoundActions[SlotToBindTo] = ActionToBind;

	//BoundActions.Add(SlotToBindTo, ActionToBind);


	OnActionBound.Broadcast(ActionToBind, SlotToBindTo);
}

bool UGameplayActionComponent::AddAttribute(FAttributeConfig NewAttribute)
{
	//check the existing attributes for one with the same tag.
	for (UActionAttribute* Attribute : Attributes)
	{
		if (Attribute->GetAttributeTag() == NewAttribute.AttributeName)
		{
			//return false if an attribute with this tag exists
			return false;
		}
	}
	UActionAttribute* ANewAttribute = UActionAttribute::CreateAttribute(this, NewAttribute.AttributeName, NewAttribute.DefaultValue);

	Attributes.Add(ANewAttribute);

	if (Attributes.Contains(ANewAttribute))
	{
		OnAttributeAdded.Broadcast(this, ANewAttribute);
	}
	
	return true;
	
}

bool UGameplayActionComponent::RemoveAttributeByTag(FGameplayTag AttributeTag)
{
	UActionAttribute* AttributeToRemove = nullptr;
	for (UActionAttribute* Attribute : Attributes)
	{
		if (Attribute->GetAttributeTag() == AttributeTag)
		{
			AttributeToRemove = Attribute;
		}
	}
	if (AttributeToRemove)
	{
		Attributes.Remove(AttributeToRemove);
		OnAttributeRemoved.Broadcast(this, AttributeToRemove);
		return true;
	}
	return false;
}

UActionAttribute* UGameplayActionComponent::GetAttributeByTag(FGameplayTag AttributeTag)
{
	for (UActionAttribute* Attribute : Attributes)
	{
		if (Attribute->GetAttributeTag() == AttributeTag)
		{
			//return false if an attribute with this tag exists
			return Attribute;
		}
	}

	return nullptr;
}

TScriptInterface<IActionOwnerInterface> UGameplayActionComponent::GetActionOwner()
{
	//need to check if the owner actually implements this interface

	if (Cast<IActionOwnerInterface>(GetOwner()))
	{
		return TScriptInterface<IActionOwnerInterface>(GetOwner());
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("The parent of a gameplay action component does not implement the ActionOwnerInterface, this is a fatal error."));
		return TScriptInterface<IActionOwnerInterface>();
	}
}

TScriptInterface<IActionPlayerControlInterface> UGameplayActionComponent::GetPlayerControlInterface()
{
	//get the action owner interface from our owner
	TScriptInterface<IActionOwnerInterface> ActionOwnerInterface = GetActionOwner();

	if (!ActionOwnerInterface.GetObject())
	{
		return TScriptInterface<IActionPlayerControlInterface>();
	}

	APlayerController* PlayerController = IActionOwnerInterface::Execute_GetControllingPlayer(ActionOwnerInterface.GetObject());

	if (!PlayerController)
	{
		return TScriptInterface<IActionPlayerControlInterface>();
	}
	if (Cast<IActionPlayerControlInterface>(PlayerController))
	{
		return TScriptInterface<IActionPlayerControlInterface>(PlayerController);
	}
	else
	{
		return TScriptInterface<IActionPlayerControlInterface>();
	}

}

APlayerController* UGameplayActionComponent::GetPlayerController()
{
	UPlayerControlComponent* PCControlComp = Cast<UPlayerControlComponent>(GetOwner()->GetComponentByClass(UPlayerControlComponent::StaticClass()));

	if (PCControlComp)
	{
		return Cast<APlayerController>(PCControlComp->GetPlayerController());
	}
	else
	{
		return nullptr;
	}
}

void UGameplayActionComponent::IsDead(bool val)
{
	if (bIsDead == false && val == true)
	{
		//we are dying
		bIsDead = val; 
		OnDied.Broadcast(this);
		//remove from encounter.

		Turn->RemoveFromEncounter();

	}
	else if (bIsDead == true && val == false)
	{
		//we are resurrecting
		bIsDead = val;
		OnRevived.Broadcast(this);
	}
}

void UGameplayActionComponent::IsUnconscious(bool val)
{
	if (bIsUnconscious == false && val == true)
	{
		bIsUnconscious = val;
		OnKnockedOut.Broadcast(this);
	}
	else if (bIsUnconscious == true && val == false)
	{
		bIsUnconscious = val;
		OnRevived.Broadcast(this);
	}
}

// Called every frame
void UGameplayActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OnTick.Broadcast(DeltaTime);

	// ...
}

void UGameplayActionComponent::AddGameplayTag(FGameplayTag InTag)
{
	ActiveGameplayTags.AddTag(InTag);
	NetMulticast_OnTagAdded(InTag);
}

void UGameplayActionComponent::RemoveGameplayTag(FGameplayTag InTag)
{
	ActiveGameplayTags.RemoveTag(InTag);
	NetMulticast_OnTagRemoved(InTag);
}

void UGameplayActionComponent::AddGameplayTags(FGameplayTagContainer InTagContainer)
{
	ActiveGameplayTags.AppendTags(InTagContainer);
	NetMulticast_OnTagsAdded(InTagContainer);
}

void UGameplayActionComponent::RemoveGameplayTags(FGameplayTagContainer InTagContainer)
{
	ActiveGameplayTags.RemoveTags(InTagContainer);
	NetMulticast_OnTagsRemoved(InTagContainer);
}

void UGameplayActionComponent::NetMulticast_OnTagAdded_Implementation(FGameplayTag TagAdded)
{
	OnTagAdded.Broadcast(this, TagAdded);
}

void UGameplayActionComponent::NetMulticast_OnTagRemoved_Implementation(FGameplayTag TagRemoved)
{
	OnTagRemoved.Broadcast(this, TagRemoved);
}

void UGameplayActionComponent::NetMulticast_OnTagsAdded_Implementation(FGameplayTagContainer TagsAdded)
{
	OnTagsAdded.Broadcast(this, TagsAdded);
}

void UGameplayActionComponent::NetMulticast_OnTagsRemoved_Implementation(FGameplayTagContainer TagsRemoved)
{
	OnTagsRemoved.Broadcast(this, TagsRemoved);
}

ARPGAIController* UGameplayActionComponent::GetAIController()
{
	//Try to cast the owner to a pawn
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		UE_LOG(LogRPG, Warning, TEXT("Tried to get AI Controller from an Action Component whose parent %s is not a subclass of Pawn. Some Actions won't work"), *GetOwner()->GetName());
		return nullptr;
	}
	ARPGAIController* AIController = Cast<ARPGAIController>(Pawn->GetController());
	if (!AIController)
	{
		UE_LOG(LogRPG, Warning, TEXT("Tried to get AI Controller from an Action Component whose parent %s is not controlled by ARPGAIController. Some Actions won't work"), *GetOwner()->GetName());
		return nullptr;
	}
	return AIController;
}

UAction_Move* UGameplayActionComponent::GetMoveAction()
{
	for (UAction* Action : CharacterActions)
	{
		if (Cast<UAction_Move>(Action))
		{
			return Cast<UAction_Move>(Action);
		}
	}
	return nullptr;
}

UGameplayActionComponent* UGameplayActionComponent::GetComponentFromActor(AActor* InActor)
{
	UGameplayActionComponent* TargetComponent = nullptr;
	if (InActor)
	{
		TargetComponent = Cast<UGameplayActionComponent>(InActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	}
	return TargetComponent;
}

void UGameplayActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UGameplayActionComponent, ActiveGameplayTags);
	DOREPLIFETIME(UGameplayActionComponent, AttributeSet);
	DOREPLIFETIME(UGameplayActionComponent, Attributes);
	DOREPLIFETIME(UGameplayActionComponent, CharacterActions);
	DOREPLIFETIME(UGameplayActionComponent, ActiveAction);
	DOREPLIFETIME(UGameplayActionComponent, BoundActions);
	DOREPLIFETIME(UGameplayActionComponent, Turn);
}

bool UGameplayActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	WroteSomething |= Channel->ReplicateSubobjectList(Attributes, *Bunch, *RepFlags);
	WroteSomething |= Channel->ReplicateSubobjectList(CharacterActions, *Bunch, *RepFlags);

	return WroteSomething;
}

