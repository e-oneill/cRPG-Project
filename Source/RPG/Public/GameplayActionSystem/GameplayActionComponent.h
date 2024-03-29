// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTaskOwnerInterface.h"
#include "GameplayTasksComponent.h"
#include "ActionAttribute.h"
#include "GameState/Turn.h"
#include "GameplayActionComponent.generated.h"

class UAction;
class UTurn;
class UAction_Move;
class ARPGAIController;
class UActionAttribute;
class UActionEffect;
class IActionPlayerControlInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayTagEventSignature, UGameplayActionComponent*, ChangedComponent, FGameplayTag, ChangedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMultiTagEventSignature, UGameplayActionComponent*, ChangedComponent, FGameplayTagContainer, ChangedTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionBindChangeEvent, UAction*, ChangedAction, int32, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionComponentTickEvent, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionComponentEventSignature, UGameplayActionComponent*, TriggerActionComponent);

//this struct is used in the details panel to set up an attribute for the component. On begin play this will be used to create an instance of an attribute
USTRUCT(BlueprintType)
struct FAttributeConfig
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag AttributeName;

	UPROPERTY(EditAnywhere)
	float DefaultValue;

	UPROPERTY(EditAnywhere)
	bool IgnoreMaxValue = false;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UGameplayActionComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	//UGameplayActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	#pragma region ActionsVariables
	//a map of actions, where the int32 is the index of the input key and hotbar slot to use for the action. (0-9)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Gameplay Action System")
	TArray<UAction*> BoundActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay Action System", Replicated)
		TArray<UAction*> CharacterActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay Action System")
		TArray<UActionEffect*> ActiveEffects;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<UAction>> DefaultActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay Action System", Replicated)
		UAction* ActiveAction;
	#pragma endregion ActionsVariables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factions", meta = (Categories = "Faction."))
	FGameplayTag Faction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Gameplay Action System")
		FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(Replicated)
		UTurn* Turn;

	#pragma region AttributesVariables
	//this is used to define an attribute configuration for the character - perhaps i could make this something you can configure in data and simply select a row?
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Gameplay Action System", meta = (TitleProperty="{AttributeName} - {DefaultValue}"))
		TArray<FAttributeConfig> AttributeSet;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay Action System")
		TArray<UActionAttribute*> Attributes;

	bool bIsUnconscious = false;
	bool bIsDead = false;
	#pragma endregion AttributesVariables


public:	
	static UGameplayActionComponent* GetActionComponent(AActor* Actor)
	{
		return (Actor) ? Cast<UGameplayActionComponent>(Actor->GetComponentByClass(UGameplayActionComponent::StaticClass())) : nullptr;
	}

	void AddActiveEffect(UActionEffect* InEffect) {ActiveEffects.Add(InEffect); }
	void RemoveActiveEffect(UActionEffect* InEffect) { ActiveEffects.Remove(InEffect); }

	UAction* GrantAction(TSubclassOf<UAction> ActionClassToGrant);
	UAction* GrantAction(UAction* ActionToGrant);

	void RemoveAction(UAction* ActionToRemove);

	void BindActionToNextFreeSlot(UAction* ActionToBind);
	void BindAction(UAction* ActionToBind, int32 SlotToBindTo);


	UPROPERTY(BlueprintAssignable)
	FActionBindChangeEvent OnActionBound;

	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnTurnAssigned;
	UPROPERTY(BlueprintAssignable)
	FTurnEventSignature OnTurnUnassigned;

	#pragma region AttributeInteractions
	//use this function whenever you want to add an attribute to a character, it will ensure that it is only added if no attribute with that tag already exists. Returns true if it was added, false if one already existed.
	bool AddAttribute(FAttributeConfig NewAttribute);
	bool AddAttribute(FGameplayTag NewAttribute, float DefaultValue, bool bDefaultToValue, bool bIgnoreMaxValue);
	bool AddAttribute(UActionAttribute* InAttribute);
	bool RemoveAttributeByTag(FGameplayTag AttributeTag);

	UFUNCTION(BlueprintCallable)
	UActionAttribute* GetAttributeByTag(FGameplayTag AttributeTag);

	//TODO - Add NetMulticast functions to wrap the AttributeAdd and Remove functions...

	UPROPERTY(BlueprintAssignable)
		FAttributeChangeSignature OnAttributeAdded;

	UPROPERTY(BlueprintAssignable)
		FAttributeChangeSignature OnAttributeRemoved;

	UPROPERTY(BlueprintAssignable)
		FActionComponentEventSignature OnKnockedOut;

	UPROPERTY(BlueprintAssignable)
		FActionComponentEventSignature OnDied;

	UPROPERTY(BlueprintAssignable)
		FActionComponentEventSignature OnRevived;

	#pragma endregion AttributeInteractions

	#pragma region GameplayTagInteractions
	void AddGameplayTag(FGameplayTag InTag);
	void RemoveGameplayTag(FGameplayTag InTag);

	void AddGameplayTags(FGameplayTagContainer InTagContainer);
	void RemoveGameplayTags(FGameplayTagContainer InTagContainer);

	#pragma region GameplayTagEvents
	//event dispatchers for gameplay tags. Mostly used to subscribe UI to changes
	UPROPERTY(BlueprintAssignable)
	FGameplayTagEventSignature OnTagAdded;
	UPROPERTY(BlueprintAssignable)
	FGameplayTagEventSignature OnTagRemoved;
	UPROPERTY(BlueprintAssignable)
	FMultiTagEventSignature OnTagsAdded;
	UPROPERTY(BlueprintAssignable)
	FMultiTagEventSignature OnTagsRemoved;

	//use these to fire an event to all clients to notify them of the change
	//only should be used for UI updates to any connected clients, as they are unreliable. Game logic will replicate separately through GetLifetimeReplicatedProps
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_OnTagAdded(FGameplayTag TagAdded);
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_OnTagRemoved(FGameplayTag TagRemoved);
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_OnTagsAdded(FGameplayTagContainer TagsAdded);
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_OnTagsRemoved(FGameplayTagContainer TagsRemoved);
	#pragma endregion GameplayTagEvents

	#pragma endregion GameplayTagInteractions
	
	#pragma region GettersSetters
	UFUNCTION(BlueprintPure)
	ARPGAIController* GetAIController();
	UFUNCTION(BlueprintPure)
	UAction_Move* GetMoveAction();
	UFUNCTION(BlueprintPure)
	UTurn* GetTurn() const { return Turn; }
	UFUNCTION(BlueprintCallable)
	void SetTurn(UTurn* val) 
	{ 
		Turn = val; 
		if (val == nullptr)
		{
			OnTurnUnassigned.Broadcast(nullptr);
		}
		else
		{
			OnTurnAssigned.Broadcast(Turn);
		}
	}
	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetActiveGameplayTags() const { return ActiveGameplayTags; }
	UFUNCTION(BlueprintCallable)
	void SetActiveGameplayTags(FGameplayTagContainer val) { ActiveGameplayTags = val; }

	UFUNCTION(BlueprintPure)
	static UGameplayActionComponent* GetComponentFromActor(AActor* InActor);

	TArray<UAction*> GetBoundActions() const {return BoundActions; }
	void SetBoundActions(TArray<UAction*> val) { BoundActions = val; }
	UAction* GetActiveAction() const { return ActiveAction; }
	void SetActiveAction(UAction* val) { ActiveAction = val; }

	UFUNCTION(BlueprintPure)
		TScriptInterface<IActionOwnerInterface> GetActionOwner();

	UFUNCTION(BlueprintPure)
		TScriptInterface<IActionPlayerControlInterface> GetPlayerControlInterface();


		APlayerController* GetPlayerController();

	UFUNCTION(BlueprintPure)
		TScriptInterface<IGameplayTaskOwnerInterface> GetTaskOwner() { return TScriptInterface<IGameplayTaskOwnerInterface>(this); }

	FGameplayTag GetFaction() const { return Faction; }
	void SetFaction(FGameplayTag val) { Faction = val; }
	TArray<UActionAttribute*> GetAttributes() const { return Attributes; }
	TArray<UAction*> GetCharacterActions() const { return CharacterActions; }
	void SetCharacterActions(TArray<UAction*> val) { CharacterActions = val; }
	bool IsDead() const { return bIsDead; }
	void IsDead(bool val); 
	bool IsUnconscious() const { return bIsUnconscious; }
	void IsUnconscious(bool val);
	#pragma endregion GettersSetters

	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//use this tick event to subscribe children of this component to tick events from the component
	UPROPERTY(BlueprintAssignable)
	FActionComponentTickEvent OnTick;

	#pragma region Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	#pragma endregion Replication

	
};
