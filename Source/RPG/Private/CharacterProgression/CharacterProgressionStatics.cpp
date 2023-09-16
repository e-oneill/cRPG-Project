// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterProgression/CharacterProgressionStatics.h"
#include "GameFramework/RPGGameState.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

TOptional<FSkillData> UCharacterProgressionStatics::GetSkillDataFromTag(FGameplayTag Skill, UWorld* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get skill data from tag, but world context was not provided"));
		return TOptional<FSkillData>();
	}

	ARPGGameState* GameState = Cast<ARPGGameState>(WorldContext->GetGameState());

	if (!GameState)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get game state, but the correct subclass is not in use"));
		return TOptional<FSkillData>();
	}
	
	USkillDataAsset* SkillDataContainer = GameState->GetSkillsData();
	if (!SkillDataContainer)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get skill data from tag, but there was no skill data container"));
		return TOptional<FSkillData>();
	}

	TOptional<FSkillData> SkillData = SkillDataContainer->GetSkillData(Skill);
	return SkillData;
}

float UCharacterProgressionStatics::CalculateSkillModifierForActor(FSkillData& Skill, UGameplayActionComponent* ActionComponent)
{
	TArray<FGameplayTag> GoverningAbilities = Skill.GoverningAbilities;
	float modifier = 0;
	for (FGameplayTag Ability : GoverningAbilities)
	{
		UActionAttribute* AbilityAttribute = ActionComponent->GetAttributeByTag(Ability);
		if (Skill.SkillType == ESkillType::Preferred)
		{
			if (AbilityAttribute->GetAttributeValue() > modifier)
			{
				modifier = AbilityAttribute->GetAttributeValue();
			}
		}
		else if (Skill.SkillType == ESkillType::Hybrid)
		{
			modifier += AbilityAttribute->GetAttributeValue();
		}
		else
		{
			modifier = AbilityAttribute->GetAttributeValue();
		}
	}

	if (Skill.SkillType == ESkillType::Hybrid)
	{
		modifier /= GoverningAbilities.Num();
		
	}

	return modifier;
}
