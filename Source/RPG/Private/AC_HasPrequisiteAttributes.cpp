// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_HasPrequisiteAttributes.h"
#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionSystemStatics.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

bool UAC_HasPrequisiteAttributes::CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/, UAction* Action /*= nullptr*/)
{
	//loop through the action's effects, and if any of them are marked as a prerequisite, check that it is met
	for (const FEffectConfigurationData& EffectConfig : Action->GetActionEffects())
	{
		if (EffectConfig.bTreatEffectAsPrerequisite)
		{
			UGameplayActionComponent* ComponentToCheck = Actor;
			//check if the target has this attribute
			if (EffectConfig.Target == EActionEffectTarget::Target)
			{
				//if the target actor has a GAC, we can check that. if they don't, return false
				if (TargetActor->GetComponentByClass(UGameplayActionComponent::StaticClass()))
				{
					ComponentToCheck = Cast<UGameplayActionComponent>(TargetActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
				}
				else
				{
					return false;
				}
			}
			//check the ActionComponent for the Attribute and check attribute value
			UActionAttribute* Attribute = ComponentToCheck->GetAttributeByTag(EffectConfig.Attribute);
			if (Attribute)
			{
				//depending on how the effect magnitude is calculated, we use a different number
				float MagnitudeToCheck;
				if (EffectConfig.EffectType == EActionEffectMagnitudeType::Fixed)
				{
					MagnitudeToCheck = EffectConfig.EffectMagnitude;
				}
				else
				{
					//if the effect uses a random in range magnitude, check upper bound
					MagnitudeToCheck = EffectConfig.EffectMaxMagnitude;
				}
				bool bHasEnoughOfAttribute = Attribute->GetAttributeValue() >= MagnitudeToCheck;
				//if at any point we find an attribute missing, return false
				if (!bHasEnoughOfAttribute)
				{
					return false;
				}
			}
			
		}
	}
	return true;
}

