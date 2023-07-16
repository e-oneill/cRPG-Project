// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionEffect.h"
#include "../RPG.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "GameState/Turn.h"
#include "GameFramework/RPGGameState.h"
#include "Components/SphereComponent.h"

UActionEffect* UActionEffect::CreateActionEffect(FEffectConfigurationData ConfigData, UGameplayActionComponent* InSource /*= nullptr*/, UGameplayActionComponent* InTarget /*= nullptr*/, FVector InTargetLocation /*= FVector::ZeroVector*/)
{
	//Generate the name for this action effect, using the target actor name if one is set, and the location if it is not
	FString Name;
	if (InTarget)
	{
		Name = (TEXT("ActionEffect_%s_%s"), *InSource->GetOwner()->GetName(), *InTarget->GetOwner()->GetName());
	}
	else
	{
		Name = (TEXT("ActionEffect_%s_%s"), *InSource->GetOwner()->GetName(), *InTargetLocation.ToString());
	}
	UActionEffect* NewActionEffect = NewObject<UActionEffect>(InSource, FName(Name));

	//call the initialization function before returning
	NewActionEffect->InitializeEffect(ConfigData, InTarget, InSource);

	return NewActionEffect;
}

void UActionEffect::InitializeEffect(FEffectConfigurationData ConfigData, UGameplayActionComponent* InTarget /*= nullptr*/, UGameplayActionComponent* InSource /*= nullptr*/)
{
	//EffectConfiguration = ConfigData;
	EffectSource = InSource;
	EffectTarget = InTarget;

	ExecutionType = ConfigData.Execution;

	//get the gameplay tag for the attribute this effect targets
	TargetAttribute = ConfigData.Attribute;
	DamageType = ConfigData.DamageType;

	//get the magnitude type so we can set the float variables
	MagnitudeType = ConfigData.EffectType;
	if (MagnitudeType == EActionEffectMagnitudeType::Fixed)
	{
		EffectMagnitude = ConfigData.EffectMagnitude;
	}
	else
	{
		EffectMinMagnitude = ConfigData.EffectMinMagnitude;
		EffectMaxMagnitude = ConfigData.EffectMaxMagnitude;

		

	}
	if (ExecutionType == EActionEffectExecutionType::Continuous || ExecutionType == EActionEffectExecutionType::OnTurn)
	{
		bExecutesOnApplication = ConfigData.bExecutesOnInit;
	}
	if (ExecutionType == EActionEffectExecutionType::Continuous)
	{
		//set up variables for a continuous effect
		DurationSeconds = ConfigData.DurationSeconds;
		//assuming turns are 6 seconds each
		DurationTurns = DurationSeconds / 6;
		EffectInterval = ConfigData.EffectInterval;
		//for simplicity, assign turn start effect
		ExecutesOnTurnEvent = EActionEffectTurnEffectType::Start;
	}
	else if (ExecutionType == EActionEffectExecutionType::OnTurn)
	{
		//set up variables for an OnTurn effect
		ExecutesOnTurnEvent = ConfigData.TurnExecutionTime;
		DurationTurns = ConfigData.DurationInTurns;
		DurationSeconds = DurationTurns * 6;
		EffectInterval = 6.f;
	}
	//if the config indicates this applies a status, get that gameplay tag
	//STATUS IS ONLY A PLACEHOLDER FOR NOW, THE SIMPLE GAMEPLAY TAG APPROACH MAY NOT WORK
	//E.G. HOW TO DO STACKING STATUSES, STATUS LENGTH, CHECK IF A STATUS ALREADY APPLIES, HOW LONG IT SHOULD LAST,
	//ETC.
	if (ConfigData.bAppliesStatus)
	{
		AppliedStatus = ConfigData.Status;
	}

	EffectTarget->AddActiveEffect(this);
	
		//singular target
		
	

}

void UActionEffect::TriggerEffect()
{
	if (bIsTriggered)
	{
		//effects should only be triggered once, return
		return;
	}
	bIsTriggered = true;
	//if this is an instant effect, we have nothing consider, just apply
	if (ExecutionType == EActionEffectExecutionType::Instant)
	{
		ApplyEffect();
		return;
	}

	//in the case of effects over time, it is more complex.
	//if the target of the effect is in turn based mode, we need to temporarily force it to fire on turns if it's continuous
	
	if (EffectTarget->GetTurn())
	{
		TargetTurn = EffectTarget->GetTurn();

		BindToTurnEvents(TargetTurn);
	}
	else
	{
		//it is currently not turn-based mode, we fire on seconds
		BindToTimer();
	}
}

void UActionEffect::TimerCallback()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeSinceTimerSet = CurrentTime - TimeStarted;
	SecondsActive += TimeSinceTimerSet;
	TimeStarted = CurrentTime;

	ApplyEffect();

	if (SecondsActive >= DurationSeconds)
	{
		//GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		PrepareToDestroy();
		//Destroy();
	}
}

void UActionEffect::BindToTimer()
{
	if (TurnsActive > 0)
	{
		SecondsActive += (TurnsActive * 6.f);
	}

	float FirstDelay;
	if (bExecutesOnApplication)
	{
		FirstDelay = 0.f;
	}
	else
	{
		FirstDelay = EffectInterval;
	}
	FuncDelegate;
	FuncDelegate.BindUFunction(this, FName("TimerCallback"));
	GetWorld()->GetTimerManager().SetTimer(EffectTickTimer, FuncDelegate, EffectInterval, true, FirstDelay);
	TimeStarted = GetWorld()->GetTimeSeconds();
}

void UActionEffect::TurnApplyCallback(UTurn* Turn)
{
	TurnsActive++;
	SecondsActive += 6;

	if (TurnsActive < DurationTurns)
	{
		ApplyEffect();
	}
	else
	{
		PrepareToDestroy();
		//Destroy();
	}
}

void UActionEffect::OnTurnAssigned(UTurn* Turn)
{
	//stop the timer
	GetWorld()->GetTimerManager().ClearTimer(EffectTickTimer);

	//
	TurnsActive += (DurationSeconds / 6);
	TargetTurn = Turn;
	BindToTurnEvents(Turn);

}

void UActionEffect::OnTurnUnassigned(UTurn* Turn)
{
	UnbindFromTurnEvents(TargetTurn);
	TargetTurn = nullptr;
	BindToTimer();
}

void UActionEffect::BindToTurnEvents(UTurn* Turn)
{
	if (ExecutesOnTurnEvent == EActionEffectTurnEffectType::Start || ExecutesOnTurnEvent == EActionEffectTurnEffectType::Both)
	{
		//bind to turn start
		Turn->OnTurnStart.AddUniqueDynamic(this, &UActionEffect::TurnApplyCallback);
	}
	else if (ExecutesOnTurnEvent == EActionEffectTurnEffectType::End || ExecutesOnTurnEvent == EActionEffectTurnEffectType::Both)
	{
		//bind to turn end
		Turn->OnTurnEnd.AddUniqueDynamic(this, &UActionEffect::TurnApplyCallback);
	}

	if (bExecutesOnApplication)
	{
		ApplyEffect();
	}
}

void UActionEffect::UnbindFromTurnEvents(UTurn* Turn)
{
	Turn->OnTurnStart.RemoveAll(this);
	Turn->OnTurnEnd.RemoveAll(this);
}

void UActionEffect::ApplyEffect_Implementation()
{
	
	UActionAttribute* Attribute = EffectTarget->GetAttributeByTag(TargetAttribute);
	if (!Attribute)
	{
		UE_LOG(LogRPG, Warning, TEXT("Tried to apply effect to target, but this target does not have the targeted attribute"));
		return;
	}

	float CurrentValue = 0;
	//if this is a fixed one modify the value by the magnitude
	if (MagnitudeType == EActionEffectMagnitudeType::Fixed)
	{
		
		CurrentValue = EffectMagnitude;
		
	}
	else if (MagnitudeType == EActionEffectMagnitudeType::RandomRange)
	{
		float RandomDamage = ARPGGameState::GetRandomFloatInRange(EffectMinMagnitude, EffectMaxMagnitude);
		RandomDamage = FMath::RoundToInt32(RandomDamage);
		CurrentValue = RandomDamage;
		
	}
	//Attribute->SetAttributeValue(CurrentValue);
	Attribute->ChangeAttributeValue(CurrentValue, this);
	

	//if this effect is instant, destroy itself
	if (ExecutionType == EActionEffectExecutionType::Instant)
	{
		PrepareToDestroy();
	}
}

void UActionEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UActionEffect, EffectSource);
	DOREPLIFETIME(UActionEffect, EffectTarget);
}

void UActionEffect::BeginDestroy()
{
	Super::BeginDestroy();
}

void UActionEffect::PrepareToDestroy()
{
	if (EffectTarget)
	{
		EffectTarget->RemoveActiveEffect(this);
	}
	if (TargetTurn)
	{
		UnbindFromTurnEvents(TargetTurn);
	}
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
}

