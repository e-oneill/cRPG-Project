#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "RPGAIController.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/Actions/Action_Move.h"
#include "GameFramework/RPGGameState.h"
#include "WorldModel.generated.h"



USTRUCT(BlueprintType)
struct FWorldModelAttribute
{
	GENERATED_BODY();

	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FGameplayTag AttributeTag;*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float AttributeValue;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float AttributeMax;
};


USTRUCT(BlueprintType)
struct FWorldModelActor
{
	GENERATED_BODY();

	FWorldModelActor()
	{

	}

	FWorldModelActor(FVector InLocation, UGameplayActionComponent* InActionComponent)
	{
		Location = InLocation;
		Faction = InActionComponent->GetFaction();
		ActionComponent = InActionComponent;
		for (UActionAttribute* Attribute : InActionComponent->GetAttributes())
		{
			FWorldModelAttribute AttributeWrapper;
			AttributeWrapper.AttributeValue = Attribute->GetAttributeValue();
			AttributeWrapper.AttributeMax = Attribute->GetAttributeBaseValue();

			AttributeMap.Add(Attribute->GetAttributeTag(), AttributeWrapper);
		}
	}

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
		FVector Location;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
		FGameplayTagContainer Statuses;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
		TMap<FGameplayTag, FWorldModelAttribute> AttributeMap;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
		FGameplayTag Faction;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
		UGameplayActionComponent* ActionComponent;

	float GetMagnitudeForEffect(FEffectConfigurationData ActionEffect)
	{
		float EffectMagnitude;
		if (ActionEffect.EffectType == EActionEffectMagnitudeType::Fixed)
		{
			EffectMagnitude = ActionEffect.EffectMagnitude;
		}
		else
		{
			EffectMagnitude = (ActionEffect.EffectMaxMagnitude + ActionEffect.EffectMinMagnitude) / 2;
		}
		return EffectMagnitude;
	}

	bool CheckCanExecuteAction(UAction* ActionToExecute, FWorldModelActor& TargetActor)
	{
		//check considerations and prerequisites
		//check if target is a valid target
		
		ARPGGameState* GameState = ActionToExecute->GetWorld()->GetGameState<ARPGGameState>();
		bool bIsHostile = GameState->IsFactionHostile(TargetActor.Faction, this->Faction);
		switch (ActionToExecute->GetValidTargets())
		{
		case EActionValidTargets::ALL:
			break;
		case EActionValidTargets::ONLYHOSTILE:
			if (!bIsHostile)
			{
				return false;
			}
			break;
		case EActionValidTargets::FRIENDLYNOTSELF:
			if (bIsHostile || this->ActionComponent == TargetActor.ActionComponent)
			{
				return false;
			}
			break;
		case EActionValidTargets::FRIENDLYINCSELF:
			if (bIsHostile)
			{
				return false;
			}
			break;
		case EActionValidTargets::ONLYSELF:
			if (this->ActionComponent != TargetActor.ActionComponent)
			{
				return false;
			}
			break;
		default:
			break;

		}

		
		if (!CheckCanExecuteAction(ActionToExecute) || Cast<UAction_Move>(ActionToExecute))
		{
			
			return CheckCanExecuteAction(ActionToExecute);
		}

		bool bInRange = FVector::Dist(Location, TargetActor.Location) <= ActionToExecute->GetActionRange();

		return bInRange;
	}

	bool CheckCanExecuteAction(UAction* ActionToExecute)
	{
		//check considerations using the data in the world model (currently not possible as considerations might use TargetActor->GetActorLocation - need to change to allow me to pass in data)

		//if any of the prequisite attributes are not available, we cannot execute effect
		TArray<FEffectConfigurationData> ActionEffects = ActionToExecute->GetActionEffects();
		for (const FEffectConfigurationData& ActionEffect : ActionEffects)
		{
			if (ActionEffect.bTreatEffectAsPrerequisite)
			{
				if (!AttributeMap.Contains(ActionEffect.Attribute) || AttributeMap[ActionEffect.Attribute].AttributeValue < GetMagnitudeForEffect(ActionEffect))
				{
					return false;
				}
			}
		}


		return true;
		
	}

	void ApplyEffectToActor(FEffectConfigurationData ActionEffect)
	{
		//if this actor has the attribute this effect impacts, apply the effect
		if (AttributeMap.Contains(ActionEffect.Attribute))
		{
			AttributeMap[ActionEffect.Attribute].AttributeValue -= GetMagnitudeForEffect(ActionEffect);
		}

		if (ActionEffect.bAppliesStatus)
		{
			Statuses.AddTag(ActionEffect.Status);
		}
	}

	FPlanEntry UseActionOnTargetActor(FWorldModelActor& Target, UAction* Action)
	{
		if (Cast<UAction_Move>(Action))
		{
			return UseMoveActionOnTargetLocation(Target.Location, Cast<UAction_Move>(Action));
		}

		TArray<FEffectConfigurationData> ActionEffects = Action->GetActionEffects();
		for (const FEffectConfigurationData& ActionEffect : ActionEffects)
		{
			if (ActionEffect.Target == EActionEffectTarget::Self)
			{
				ApplyEffectToActor(ActionEffect);
			}
			else
			{
				Target.ApplyEffectToActor(ActionEffect);
			}
			

			//for now, not handling area of effect skills, just treating them as if they hit the target - that will need to be fixed later
		}

		FPlanEntry PlanEntry;
		PlanEntry.Action = Action;
		PlanEntry.TargetActor = Target.ActionComponent;
		return PlanEntry;
	}

	//how to handle move? special method?
	FPlanEntry UseMoveActionOnTargetLocation(FVector TargetLocation, UAction_Move* MoveAction)
	{
		float MovementRemaining = MoveAction->GetActionRange();

		float DistanceToTarget = FVector::Dist(TargetLocation, Location);

		//just use a simple comparison instead of tracing a path for performance reasons
		//may cause issues if the ai makes a plan that involves moving somewhere impassible
		//Add check that a path does exist?
		if (DistanceToTarget <= MovementRemaining)
		{
			Location = TargetLocation;
		}
		else
		{
			FVector Line = TargetLocation - Location;
			Line.Normalize();
			FVector LocationAtDistance = Location + (Line * MovementRemaining);
			Location = LocationAtDistance;
		}


		FPlanEntry PlanEntry;
		PlanEntry.Action = MoveAction;
		PlanEntry.TargetLocation = Location;

		return PlanEntry;
	}

	bool operator==(const FWorldModelActor& lhs)
	{
		return this->ActionComponent == lhs.ActionComponent;
	}

};

USTRUCT(BlueprintType)
struct  FWorldModel
{
	GENERATED_BODY();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FWorldModelActor Self;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TArray<FWorldModelActor> KnownActors;

	TArray<UAction*> ActionsApplied;

	int32 CurrentActionIndex;

	AAIController* AIController;

	FWorldModel* CreateWorldModel(AAIController* InAIController)
	{
		AIController = InAIController;
		AActor* MyActor = Cast<AActor>(AIController->GetPawn());
		if (MyActor)
		{
			UGameplayActionComponent* MyActionComponent = Cast<UGameplayActionComponent>(MyActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
			if (MyActionComponent)
			{	
				Self = FWorldModelActor(MyActor->GetActorLocation(), MyActionComponent);
			}
			
		}


		ARPGAIController* RPGAIController = Cast<ARPGAIController>(AIController);
		if (RPGAIController)
		{
			for (const FSpottedCharacter& SpottedChar : RPGAIController->GetSpottedCharacters())
			{
				if (SpottedChar.SpottedActionComponent)
				{
					FWorldModelActor SpottedActor = FWorldModelActor(SpottedChar.SpottedActionComponent->GetOwner()->GetActorLocation(), SpottedChar.SpottedActionComponent);
					KnownActors.Add(SpottedActor);
				}
			}
		}


		return this;
	}

	FWorldModelActor& GetHighestPriorityTarget()
	{
		//if we don't know any actors, return the self ref
		if (KnownActors.Num() == 0)
		{
			return Self;
		}

		if (!AIController || !AIController->GetPawn()->GetWorld()->GetGameState<ARPGGameState>())
		{
			return KnownActors[0];
		}

		ARPGGameState* GameState = AIController->GetPawn()->GetWorld()->GetGameState<ARPGGameState>();

		//for now just return the first hostile actor we know about
		for (FWorldModelActor& KnownActor : KnownActors)
		{
			if (GameState->IsFactionHostile(KnownActor.Faction, Self.Faction))
			{
				return KnownActor;
			}
		}


		return Self;
	}
};