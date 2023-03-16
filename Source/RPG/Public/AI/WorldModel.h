#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "RPGAIController.h"
#include "GameplayActionSystem/Action.h"
#include "WorldModel.generated.h"

USTRUCT(BlueprintType)
struct FWorldModelAttribute
{
	GENERATED_BODY();
	UPROPERTY(VisibleInstanceOnly)
	FGameplayTag AttributeTag;
	UPROPERTY(VisibleInstanceOnly)
	float AttributeValue;
};


USTRUCT(BlueprintType)
struct FWorldModelActor
{
	GENERATED_BODY();

	FWorldModelActor()
	{

	}

	FWorldModelActor(FVector InLocation, FGameplayTag InFaction, TArray<UActionAttribute*> InAttributes)
	{
		Location = InLocation;
		Faction = InFaction;
		for (UActionAttribute* Attribute : InAttributes)
		{
			/*FWorldModelAttribute ModelAttribute;
			ModelAttribute.AttributeTag = Attribute->GetAttributeTag();
			ModelAttribute.AttributeValue = Attribute->GetAttributeValue();
			Attributes.Add(ModelAttribute);*/

			AttributeMap.Add(Attribute->GetAttributeTag(), Attribute->GetAttributeValue());
			
		}
	}

	UPROPERTY(VisibleInstanceOnly)
	FVector Location;
	FGameplayTagContainer Statuses;
	TMap<FGameplayTag, float> AttributeMap;
	FGameplayTag Faction;

	bool CheckCanExecuteAction(UAction* ActionToExecute)
	{
		//check all action effects that are prerequisites

		//check considerations using the data in the world model (currently not possible as considerations might use TargetActor->GetActorLocation - need to change to allow me to pass in data)
	}

	void ApplyEffectToActor(FEffectConfigurationData ActionEffect)
	{
		//if this actor has the attribute this effect impacts, apply the effect
		if (AttributeMap.Contains(ActionEffect.Attribute))
		{
			float EffectMagnitude;
			//check the effect type if instant 
			if (ActionEffect.EffectType == EActionEffectMagnitudeType::Fixed)
			{
				EffectMagnitude = ActionEffect.EffectMagnitude;
			}
			else
			{
				EffectMagnitude = (ActionEffect.EffectMaxMagnitude + ActionEffect.EffectMinMagnitude) / 2;
			}

			AttributeMap[ActionEffect.Attribute] -= EffectMagnitude;
		}

		if (ActionEffect.bAppliesStatus)
		{
			Statuses.AddTag(ActionEffect.Status);
		}
	}

	//use Deduct Action Costs from Actor when you want to apply the costs of using an action to the actor
	/*void DeductActionCostsFromActor(UAction* ActionToUse)
	{
		TArray<FEffectConfigurationData> ActionEffects = ActionToUse->GetActionEffects();
		for (FEffectConfigurationData ActionEffect : ActionEffects)
		{
			if (!ActionEffect.bTreatEffectAsPrerequisite)
			{
				continue;
			}
			ApplyEffectToActor(ActionEffect);
		}
	}*/

	//use Apply Action to Actor where you are applying the action to its target (may be the same actor as the source)
	/*void ApplyActionToActor(UAction* ActionToApply)
	{
		//get the effects that this action applies and then apply them to the attributes of our model actor
		TArray<FEffectConfigurationData> ActionEffects = ActionToApply->GetActionEffects();
		for (FEffectConfigurationData ActionEffect : ActionEffects)
		{
			if (ActionEffect.bTreatEffectAsPrerequisite)
			{
				//do not apply prerequisite effects to an actor - these should be applied using the DeductActionCostsFromActor
				continue;
			}
			ApplyEffectToActor(ActionEffect);

		}
	}*/

	void UseActionOnTargetActor(FWorldModelActor& Target, UAction* Action)
	{
		TArray<FEffectConfigurationData> ActionEffects = Action->GetActionEffects();
		for (FEffectConfigurationData ActionEffect : ActionEffects)
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
	}

};

USTRUCT(BlueprintType)
struct  FWorldModel
{
	GENERATED_BODY();

	UPROPERTY(VisibleInstanceOnly)
	FWorldModelActor Self;

	UPROPERTY(VisibleInstanceOnly)
	TArray<FWorldModelActor> KnownActors;

	FWorldModel* CreateWorldModel(AAIController* AIController)
	{
		
		AActor* MyActor = Cast<AActor>(AIController->GetPawn());
		if (MyActor)
		{
			UGameplayActionComponent* MyActionComponent = Cast<UGameplayActionComponent>(MyActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
			if (MyActionComponent)
			{	
				Self = FWorldModelActor(MyActor->GetActorLocation(), MyActionComponent->GetFaction(), MyActionComponent->GetAttributes());
			}
			
		}


		ARPGAIController* RPGAIController = Cast<ARPGAIController>(AIController);
		if (RPGAIController)
		{
			for (FSpottedCharacter SpottedChar : RPGAIController->GetSpottedCharacters())
			{
				if (SpottedChar.SpottedActionComponent)
				{
					FWorldModelActor SpottedActor = FWorldModelActor(SpottedChar.GetLastSpottedLocation(), SpottedChar.SpottedActionComponent->GetFaction(), SpottedChar.SpottedActionComponent->GetAttributes());
					KnownActors.Add(SpottedActor);
				}
			}
		}


		return this;
	}

};