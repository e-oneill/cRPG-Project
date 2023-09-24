// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionAttribute.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/ActionEffect.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "ActionSystemTags.h"
#include "RPGGameStatics.h"
#include "CharacterProgression/CharacterProgressionStatics.h"
#include "Interfaces/CharacterProgression/ExperienceGrantable.h"


void UActionAttribute::OnRep_AttributeValue()
{
	//if this is firing, the attribute value has changed. Fire the OnChangeEvent
	OnAttributeChanged.Broadcast(ActionComponent, this);
}



UActionAttribute* UActionAttribute::CreateAttribute(UGameplayActionComponent* OwningComponent,FGameplayTag InAttributeTag, float InDefaultValue, bool bShouldDefaultToBaseValue /* = true*/, bool bIgnoreMaxValue /* = false*/)
{
	UActionAttribute* NewAttribute = NewObject<UActionAttribute>(OwningComponent, FName(InAttributeTag.ToString()));
	NewAttribute->SetActionComponent(OwningComponent);
	NewAttribute->SetAttributeTag(InAttributeTag);
	NewAttribute->SetAttributeBaseValue(InDefaultValue);
	NewAttribute->IgnoreMaxValue = bIgnoreMaxValue;
	if (bShouldDefaultToBaseValue)
	{
		NewAttribute->SetAttributeValue(InDefaultValue);
	}
	else
	{
		NewAttribute->SetAttributeValue(0.f);
	}
	return NewAttribute;
}

UActionAttribute* UActionAttribute::CreateAttribute(UGameplayActionComponent* OwningComponent, FAttributeConfig AttributeConfig)
{
	UActionAttribute* NewAttribute = CreateAttribute(OwningComponent, AttributeConfig.AttributeName, AttributeConfig.DefaultValue, true, AttributeConfig.IgnoreMaxValue);

	return NewAttribute;

}

void UActionAttribute::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UActionAttribute, AttributeTag);
	DOREPLIFETIME(UActionAttribute, AttributeBaseValue);
	DOREPLIFETIME(UActionAttribute, AttributeValue);
	DOREPLIFETIME(UActionAttribute, ActionComponent);
	DOREPLIFETIME(UActionAttribute, IgnoreMaxValue);

}

void UActionAttribute::ChangeAttributeValue(float val, UActionEffect* SourceEffect)
{
	if (AttributeTag.MatchesTagExact(FActionSystemTags::Get().Attr_Health))
	{
		HandleHealthChange(val, SourceEffect);
	}
	else
	{
		HandleStandardChange(val, SourceEffect);
	}
	//OnAttributeChanged.Broadcast(ActionComponent, this);
}

float UActionAttribute::GetSkillCheckModifier(UGameplayActionComponent* ActingComponent)
{
	//if the tag depth match is less than 2, the tag is not a child of Attribute.Skill
	if (AttributeTag.MatchesTagDepth(FActionSystemTags::Get().Skill) < 2)
	{
		return -1.f;
	}
	float AbilityModifier = 0;


	TOptional<FSkillData> OptionalSkillData = UCharacterProgressionStatics::GetSkillDataFromTag(AttributeTag, GetWorld());
	if (OptionalSkillData.IsSet())
	{
		FSkillData& SkillData = OptionalSkillData.GetValue();
		AbilityModifier = UCharacterProgressionStatics::CalculateSkillModifierForActor(SkillData, ActionComponent);
	}

	

	//for now just return the level of the skill
	return AttributeValue + AbilityModifier;
}

void UActionAttribute::HandleStandardChange(float val, UActionEffect* SourceEffect)
{
	float NewAttributeValue = AttributeValue - val;
	float max = AttributeBaseValue;
	if (IgnoreMaxValue)
	{
		max = FLT_MAX;
	}

	AttributeValue = FMath::Clamp(NewAttributeValue, 0.f, max);
	OnAttributeChanged.Broadcast(ActionComponent, this);
}

void UActionAttribute::HandleHealthChange(float val, UActionEffect* SourceEffect)
{
	if (!ActionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to do health damage to an actor with no action component!"));
		return;
	}

	UActionAttribute* PhysicalArmour = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_PhysicalArmour);
	UActionAttribute* MagicalArmour = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_MagicalArmour);
	FGameplayTag SourceDamageType = SourceEffect->GetDamageType();
	UGameplayActionComponent* DamageDealer = SourceEffect->GetEffectSource();

	if (val > 0 && PhysicalArmour && SourceDamageType.MatchesTag(FActionSystemTags::Get().Dmg_Physical))
	{
		if (PhysicalArmour->GetAttributeValue() < val)
		{
			float ArmourValue = PhysicalArmour->GetAttributeValue();
			PhysicalArmour->ChangeAttributeValue(val, SourceEffect);
			val -= ArmourValue;
		}
		else
		{
			PhysicalArmour->ChangeAttributeValue(val, SourceEffect);
			return;
		}

	}
	else if (val > 0 && MagicalArmour && SourceDamageType.MatchesTag(FActionSystemTags::Get().Dmg_Magical))
	{
		if (MagicalArmour->GetAttributeValue() < val)
		{
			float ArmourValue = MagicalArmour->GetAttributeValue();
			MagicalArmour->ChangeAttributeValue(val, SourceEffect);
			val -= ArmourValue;
		}
		else
		{
			MagicalArmour->ChangeAttributeValue(val, SourceEffect);
			return;
		}
	}

	HandleStandardChange(val, SourceEffect);

	

	if (AttributeValue <= 0)
	{
		if (Cast<IExperienceGrantable>(ActionComponent->GetOwner()) && Cast<IExperienceGrantable>(DamageDealer->GetOwner()))
		{
			//both the attacker and attacked implement the interface
			IExperienceGrantable::Execute_GrantExperience(ActionComponent->GetOwner(), DamageDealer);
		}
		UActionAttribute* ExperienceAttribute = DamageDealer->GetAttributeByTag(FActionSystemTags::Get().Attr_Experience);
		if (ExperienceAttribute)
		{

		}

		//if the attribute is Health, we should run low health logic at zero hp
		HandleDeath();
	}
}

void UActionAttribute::SetAttributeValue(float val)
{
	AttributeValue = FMath::Clamp(val, 0.f, AttributeBaseValue);
	OnAttributeChanged.Broadcast(ActionComponent, this);
}



void UActionAttribute::HandleDeath()
{
	bool bIsPlayerControlled = (URPGGameStatics::GetActionComponentPlayerController(ActionComponent) != nullptr);




	if (bIsPlayerControlled)
	{
		ActionComponent->IsUnconscious(true);	
	}
	else
	{
		ActionComponent->IsDead(true);
	}
}