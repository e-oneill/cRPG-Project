// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
//#include "ActionEffect.h"
#include "GameplayActionSystemStatics.generated.h"
/**
 * 
 */

 USTRUCT(BlueprintType)
 struct  FAttributeData
 {
	GENERATED_BODY();

	public:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	FName AttributeName;

	UPROPERTY(BlueprintReadWrite, Category = "Attribute")
	float BaseValue;

 };

 UENUM(BlueprintType)
	 enum class EActionState : uint8
 {
	 Idle,
	 Preparing,
	 Executing,
	 Complete,
	 Canceling
 };

 #pragma region ActionEffectConfig

 UENUM(BlueprintType)
	 enum class EActionEffectExecutionType : uint8
 {
	 Instant,
	 Continuous,
	 OnTurn
 };

 UENUM(BlueprintType)
	 enum class EActionEffectTurnEffectType : uint8
 {
	 Start,
	 End,
	 Both
 };


 UENUM(BlueprintType)
 enum class EActionEffectTarget : uint8
 {
	Self UMETA(DisplayName = "Self"),
	Target UMETA(DisplayName = "Target"),
	AreaSelf UMETA(DisplayName = "Area (Centred on Self)"),
	AreaTarget UMETA(DisplayName = "Area (Centred on Target")
 };

 UENUM(BlueprintType)
 enum class EActionEffectMagnitudeType : uint8
 {
	Fixed UMETA(DisplayName = "Fixed"),
	RandomRange UMETA(DisplayName = "Random Within a Range")
 };

 USTRUCT(BlueprintType)
 struct FEffectConfigurationData
 {
	 GENERATED_BODY();

 public:
	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution")
	EActionEffectExecutionType Execution;

	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (ToolTip="Set this value to true to have this effect treated as a prequisite.\nThe action cannot be activated without having the resources to apply this effect"))
	bool bTreatEffectAsPrerequisite;

	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (EditCondition = "Execution == EActionEffectExecutionType::OnTurn", EditConditionHides))
	int32 DurationInTurns = 1;

	//should this effect be applied at the start of a turn, the end or both (unusual)
	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (EditCondition = "Execution == EActionEffectExecutionType::OnTurn", EditConditionHides))
	EActionEffectTurnEffectType TurnExecutionTime;

	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (Delta = 6, EditCondition = "Execution == EActionEffectExecutionType::Continuous", EditConditionHides))
	int32 DurationSeconds = 60;

	//how often should this effect apply, to make this a once-off buff with a duration, set this greater than the duration
	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (EditCondition = "Execution == EActionEffectExecutionType::Continuous", EditConditionHides))
	int32 EffectInterval = 6;

	//should this effect apply when it is fired, or only from the end of the first tick interval
	UPROPERTY(EditDefaultsOnly, Category = "EffectExecution", meta = (EditCondition = "Execution == EActionEffectExecutionType::Continuous", EditConditionHides))
	bool bExecutesOnInit = true;


	UPROPERTY(EditDefaultsOnly, Category = "Target", meta = (Categories = "Attribute."))
	FGameplayTag Attribute;

	UPROPERTY(EditDefaultsOnly, Category = "Target")
	EActionEffectTarget Target;

	UPROPERTY(EditDefaultsOnly, Category = "Target", meta = (ClampMin = 100, EditCondition = "Target == EActionEffectTarget::AreaSelf || Target == EActionEffectTarget::AreaTarget", EditConditionHides))
	float AreaOfEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Target")
	EActionEffectMagnitudeType EffectType;

	UPROPERTY(EditDefaultsOnly, Category = "Target", meta = (EditCondition = "EffectType == EActionEffectMagnitudeType::Fixed", EditConditionHides))
	float EffectMagnitude;

	UPROPERTY(EditDefaultsOnly, Category = "Target", meta = (EditCondition = "EffectType == EActionEffectMagnitudeType::RandomRange", EditConditionHides))
	float EffectMinMagnitude;

	UPROPERTY(EditDefaultsOnly, Category = "Target", meta = (EditCondition = "EffectType == EActionEffectMagnitudeType::RandomRange", EditConditionHides))
	float EffectMaxMagnitude;

	UPROPERTY(EditDefaultsOnly, Category="StatusEffect")
	bool bAppliesStatus;

	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect", meta = (Categories = "Status", EditCondition = "bAppliesStatus", EditConditionHides))
	FGameplayTag Status;

 };

#pragma endregion ActionEffectConfig


 UENUM(BlueprintType)
 enum class ECueType : uint8
 {
	Animation,
	Particle,
	Audio
 };

 UENUM(BlueprintType)
 enum class ECueRelevancy : uint8
 {
	ControllingClient,
	NetMulticast
 };

 UENUM(BlueprintType)
 enum class ECueTarget : uint8
 {
	 Source,
	 Target,
	 Location
 };

 UENUM(BlueprintType)
 enum class ECueExecuteTime : uint8
 {
	OnStartPrepare,
	OnPrepare,
	OnEndPrepare,
	OnStartExecute,
	OnExecute,
	OnEndExecute
 };

 UENUM(BlueprintType)
 enum class ECueActionStatusContext : uint8
 {
	 Always,
	 Success,
	 Failure
 };

 USTRUCT(BlueprintType)
struct FCueConfigurationData
 {
	GENERATED_BODY();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	ECueType Type;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ToolTip = "This is the Actor that the Cue will run on. Selecting Location for an Animation type Cue will make it do nothing"))
	ECueTarget Target;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ToolTip = "For networking purposes. Show the Cue only to the Controlling Client for the Character or Play it on all connected clients"))
	ECueRelevancy Relevancy;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ToolTip = "When in the Action's execution should the cue fire. The Start and End Prefixes denote that the Cue should fire once at that stage of the process. The others will result in the Cue looping throughout that Stage"))
	ECueExecuteTime ExecuteTime;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	ECueActionStatusContext ExecutesOnActionStatus;
	//should the action wait for the cue to complete before it can move state
	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ToolTip="Set whether the Action should be halted from completing until after this action completes"))
	bool bHaltActionUntilComplete = false;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (EditCondition = "Type == ECueType::Animation", EditConditionHides))
	UAnimMontage* AnimationMontage;

 };

 UENUM()
enum class EMoveTaskResult : uint8
 {
	ReachedTarget UMETA(DisplayName = "Reached Target"),
	RanOutOfMovement UMETA(DisplayName = "Ran Out of Movement"),
	Stopped UMETA(DisplayName = "Stopped By Something")
 };

 USTRUCT(BlueprintType)
struct FTaskResultData
 {
	 GENERATED_BODY();

public:
	UPROPERTY(BlueprintReadOnly)
	bool Success;
 };

 USTRUCT(BlueprintType)
 struct FMoveTaskResultData : public FTaskResultData
 {
	GENERATED_BODY();

	public:
	UPROPERTY(BlueprintReadOnly)
	EMoveTaskResult MoveResult;
 };

 USTRUCT(BlueprintType)
 struct FActionEffectWrapper
 {
	GENERATED_BODY();

public:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	//class UActionEffect* ActionEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ShowOnlyInnerProperties))
	FEffectConfigurationData EffectConfiguration;
 };

class RPG_API GameplayActionSystemStatics
{
public:
	GameplayActionSystemStatics();
	~GameplayActionSystemStatics();
};
