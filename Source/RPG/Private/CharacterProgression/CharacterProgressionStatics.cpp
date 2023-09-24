// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterProgression/CharacterProgressionStatics.h"
#include "GameFramework/RPGGameState.h"
#include "../RPG.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "ActionSystemTags.h"
#include "RPGGameStatics.h"

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
	FGameplayTagContainer GoverningAbilities = Skill.GoverningAbilities;
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

bool UCharacterProgressionStatics::CanLevelUp(UGameplayActionComponent* ToLevel)
{
	UActionAttribute* Experience = ToLevel->GetAttributeByTag(FActionSystemTags::Get().Attr_Experience);

	UActionAttribute* Level = ToLevel->GetAttributeByTag(FActionSystemTags::Get().Attr_Level);

	return CanLevelUp(Experience, Level, ToLevel->GetWorld());
}

bool UCharacterProgressionStatics::CanLevelUp(UActionAttribute* Experience, UActionAttribute* Level, UWorld* WorldContext)
{
	if (!Experience || !Level)
	{
		return false;
	}

	USkillDataAsset* GameDataAsset = GetSkillDataAsset(WorldContext);
	if (!GameDataAsset)
	{
		return false;
	}

	int XPToLevel = GameDataAsset->GetXPToLevel(Level->GetAttributeValue() + 1);

	return Experience->GetAttributeValue() >= XPToLevel;
}

void UCharacterProgressionStatics::LevelUpCharacter(UGameplayActionComponent* ToLevel)
{
	UActionAttribute* Experience = ToLevel->GetAttributeByTag(FActionSystemTags::Get().Attr_Experience);
	UActionAttribute* Level = ToLevel->GetAttributeByTag(FActionSystemTags::Get().Attr_Level);
	


	if (!CanLevelUp(Experience, Level, ToLevel->GetWorld()))
	{
		return;
	}

	//increase by one
	Level->ChangeAttributeValue(-1, nullptr);
	USkillDataAsset* GameDataAsset = GetSkillDataAsset(ToLevel->GetWorld());
	for (FLevelUpReward LevelUpReward : GameDataAsset->GetLevelUpRewards())
	{
		FGameplayTag RewardAttributeTag = LevelUpReward.AttributeToReward;
		UActionAttribute* Attribute = ToLevel->GetAttributeByTag(RewardAttributeTag);
		if (!Attribute)
		{
			ToLevel->AddAttribute(RewardAttributeTag, LevelUpReward.AmountToReward, true, true);
		}
		else
		{
			Attribute->ChangeAttributeValue(-1*LevelUpReward.AmountToReward, nullptr);
		}
	}
}

USkillDataAsset* UCharacterProgressionStatics::GetSkillDataAsset(UWorld* WorldContext)
{
	ARPGGameState* GameState = URPGGameStatics::GetGameState(WorldContext);
	if (!GameState)
	{
		return nullptr;
	}

	return GameState->GetSkillsData();
}

void UCharacterProgressionStatics::SpendPointsOnAttribute(UGameplayActionComponent* ToSpend, UActionAttribute* AttributeToIncrease, UActionAttribute* AttributeToSpend, float AmountToSpend /*= 1.0f*/)
{
	FGameplayTagContainer AttributeToIncreaseTags = AttributeToIncrease->GetAttributeTag().GetGameplayTagParents();
	FGameplayTagContainer AttributeToSpendTags = AttributeToSpend->GetAttributeTag().GetGameplayTagParents();
	FGameplayTag Last = AttributeToIncreaseTags.Last();
	FGameplayTag OtherLast = AttributeToIncreaseTags.Last();
	if (Last.MatchesTagExact(OtherLast))
	{
		AttributeToIncrease->ChangeAttributeValue(-1*AmountToSpend, nullptr);
		AttributeToSpend->ChangeAttributeValue(AmountToSpend, nullptr);
	}
}
