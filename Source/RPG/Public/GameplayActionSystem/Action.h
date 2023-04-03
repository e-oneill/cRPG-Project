// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "GameplayActionSystemStatics.h"
#include "GameplayTagContainer.h"
#include "Action.generated.h"


class UGameplayActionComponent;
class UActionConsideration;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FActionStateEventSignature, UAction*, Action, EActionState, State, EActionState, OldState);

UENUM()
enum class EActionValidTargets
{
	ALL,
	ONLYHOSTILE,
	FRIENDLYNOTSELF,
	FRIENDLYINCSELF,
	ONLYSELF
};

/**
 * This is the base class for a action
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class RPG_API UAction : public UReplicatedObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void InitializeAction(UGameplayActionComponent* InSource);

	UFUNCTION(BlueprintNativeEvent)
	void Tick();

	#pragma region ActionExecutionLifecycle
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//this method will check the considerations on this action and the blocked tags, and will return true if this action can run. Should not override in normal circumstances
	bool CanPrepareAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	//this method will call CanPrepareAction and if true, it will execute the action. This is the normal way to prepare an action. Should not override.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TryPrepareAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	//this will move the action to preparation state
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PrepareAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	//this method will check the consideration on this action and blocked tags, will return true if this action can run. Should not override in normal circumstances
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanExecuteAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	//will call canexecute first, and if it returns true, will execute. Should not override in normal circumstances
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool TryExecuteAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);
	//Can be directly called to execute the action. Can be overriden, but you should call the parent when you do.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ExecuteAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CompleteAction(FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetAction();
	#pragma endregion ActionExecutionLifecycle

	#pragma region Getters_Setters
	UGameplayActionComponent* GetSource() const { return Source; }
	void SetSource(UGameplayActionComponent* val) { Source = val; }
	EActionState GetState() const { return State; }
	void SetState(EActionState val) { State = val; }
	virtual float GetActionRange() const { return ActionRange; }
	void SetActionRange(float val) { ActionRange = val; }
	#pragma endregion Getters_Setters
	
	#pragma region Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_State(EActionState& OldState);
	#pragma endregion Replication

	#pragma region ActionEvents
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Action System")
	FActionStateEventSignature OnActionPrepare;
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Action System")
	FActionStateEventSignature OnActionExecute;
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Action System")
	FActionStateEventSignature OnActionCancel;
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Action System")
	FActionStateEventSignature OnActionComplete;
	#pragma endregion ActionEvents

	void PostInitProperties() override;

	#pragma region UIElements
	FName GetActionName() const { return ActionName; }
	void SetActionName(FName val) { ActionName = val; }
	UTexture2D* GetActionIcon() const { return ActionIcon; }
	void SetActionIcon(UTexture2D* val) { ActionIcon = val; }
	#pragma endregion UIElements


	TArray<FEffectConfigurationData> GetActionEffects() const { return ActionEffects; }
	void SetActionEffects(TArray<FEffectConfigurationData> val) { ActionEffects = val; }
	EActionValidTargets GetValidTargets() const { return ValidTargets; }
	void SetValidTargets(EActionValidTargets val) { ValidTargets = val; }

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action|UI")
	FName ActionName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action|UI")
	UTexture2D* ActionIcon;

	//set to -1 to indicate that the action has infinite range
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (ClampMin = -1.f))
	float ActionRange;

	UPROPERTY(EditAnywhere, Category="Action")
	EActionValidTargets ValidTargets;

	#pragma region GameplayTags
	//which tags will block this from executing
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Tags")
	FGameplayTagContainer GrantsTags;
	#pragma endregion GameplayTags

	#pragma region ActionTicking
	UPROPERTY(EditDefaultsOnly)
	bool bTickOnPrepare = true;
	UPROPERTY(EditDefaultsOnly)
	bool bTickOnExecute = true;
	#pragma endregion ActionTicking

	//Set this property to decide if the Actor should turn to face the target before executing this action.
	UPROPERTY(EditDefaultsOnly)
	bool bFaceTarget;
	//Set this to true to block the automatic calling of complete action by execute action. For instance, to wait for a gameplay task to complete instead.
	UPROPERTY(EditDefaultsOnly)
	bool bNoAutoStop = false;

	//Method called from Post Init Properties. Use this to set Gameplay Tags, Considerations and Effect on the Action.
	virtual void SetupActionData();

	void LookAtLocation(FVector TargetLocation, AActor* TargetActor);

	#pragma region ActionCues
	void PlayActionCues(ECueExecuteTime ExecuteTime, UGameplayActionComponent* TargetComponent, FVector TargetLocation);
	FTimerHandle ActionCueTimer;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_PlayCue(const FCueConfigurationData& CueConfig, UGameplayActionComponent* TargetComponent, const FVector& TargetLocation);
	#pragma endregion ActionCues

	FTimerDelegate GetDelegateForNextState(FVector TargetLocation, AActor* TargetActor, ECueExecuteTime ExecuteTime);

	bool CheckConsiderations(FVector TargetLocation, AActor* TargetActor, EActionState StateToCheck);
	

	#pragma region ActionComponents
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UActionConsideration>> ConsiderationTypes; 

	UPROPERTY(BlueprintReadOnly)
	TArray<UActionConsideration*> Considerations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "{Attribute} - {Target} - {EffectType} {Execution}"))
	TArray<FEffectConfigurationData> ActionEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "{Type} - {Target} - {ExecuteTime}"))
	TArray<FCueConfigurationData> ActionCues;
	#pragma endregion ActionComponents


	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Action System", Replicated)
	UGameplayActionComponent* Source;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Action System", ReplicatedUsing=OnRep_State)
	EActionState State;

};
