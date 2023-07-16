// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "GameplayActionSystemStatics.h"
#include "ActionEffect.generated.h"

class UGameplayActionComponent;
class UShapeComponent;




/**
 *	An Action Effect is a UObject through which an Action has an effect on the world
 */
UCLASS(BlueprintType, Blueprintable)
class RPG_API UActionEffect : public UReplicatedObject
{
	GENERATED_BODY()

public:
	static UActionEffect* CreateActionEffect(FEffectConfigurationData, UGameplayActionComponent* InSource = nullptr, UGameplayActionComponent* InTarget = nullptr, FVector InTargetLocation = FVector::ZeroVector);

	void InitializeEffect(FEffectConfigurationData ConfigData, UGameplayActionComponent* InTarget = nullptr, UGameplayActionComponent* InSource = nullptr);

	void TriggerEffect();

	UFUNCTION(BlueprintNativeEvent)
	void ApplyEffect();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetDamageType() const { return DamageType; };

protected:
	void PrepareToDestroy();

	bool bIsTriggered = false;
	bool bExecutesOnApplication;

	#pragma region EffectsWithDuration

	#pragma region ContinuousHelperVariables
	UFUNCTION()
	void TimerCallback();
	void BindToTimer();
	EActionEffectExecutionType ExecutionType;
	FTimerDynamicDelegate FuncDelegate;
	FTimerHandle EffectTickTimer;
	int32 DurationSeconds;
	float TimeStarted;
	float SecondsActive;
	#pragma endregion ContinuousHelperVariables
	int32 EffectInterval;

	#pragma region OnTurnHelperVariables

	UFUNCTION()
	void TurnApplyCallback(UTurn* Turn);
	UFUNCTION()
	void OnTurnAssigned(UTurn* Turn);
	UFUNCTION()
	void OnTurnUnassigned(UTurn* Turn);
	UFUNCTION()
	void BindToTurnEvents(UTurn* Turn);
	UFUNCTION()
	void UnbindFromTurnEvents(UTurn* Turn);

	UTurn* TargetTurn;
	EActionEffectTurnEffectType ExecutesOnTurnEvent;
	int32 DurationTurns;
	int32 TurnsActive;

	#pragma endregion OnTurnHelperVariables

	#pragma endregion EffectsWithDuration
	


	/*UPROPERTY(Replicated)
	FEffectConfigurationData EffectConfiguration;*/

	UPROPERTY(BlueprintReadOnly, Replicated)
	UGameplayActionComponent* EffectTarget;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UGameplayActionComponent* EffectSource;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;

	FGameplayTag TargetAttribute;

	FGameplayTag DamageType;

	//stores whether this effect should impact the target or the source
	EActionEffectTarget Target;

	EActionEffectMagnitudeType MagnitudeType;

	float EffectMagnitude;

	float EffectMinMagnitude;

	float EffectMaxMagnitude;

	FGameplayTag AppliedStatus;
};
