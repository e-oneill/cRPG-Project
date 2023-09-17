// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/GameplayActionSystemStatics.h"
#include "ActionSystemTags.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "../RPGGameMode.h"
#include "../RPG.h"


ESKillCheckResult UGameplayActionSystemStatics::MakeSkillCheck(UGameplayActionComponent* ActionComponent, FGameplayTag Skill, float TargetNumber)
{
	if (Skill.MatchesTagDepth(FActionSystemTags::Get().Skill) < 2)
	{
		return ESKillCheckResult::INVALID;
	}

	UActionAttribute* SkillAttribute = ActionComponent->GetAttributeByTag(Skill);
	float SkillModifier = 0.f;
	if (SkillAttribute)
	{
		SkillModifier += SkillAttribute->GetSkillCheckModifier(ActionComponent);
	}

	ARPGGameMode* GameMode = Cast<ARPGGameMode>(ActionComponent->GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		return ESKillCheckResult::INVALID;
	}


	float RollResult = GameMode->GetRandomInt(0, 100) + SkillModifier;

	UE_LOG(LogRPG, Log, TEXT("%s attempted skill check %s with target number %f, rolled %f"), *ActionComponent->GetOwner()->GetName(), *Skill.GetTagName().ToString(), TargetNumber, RollResult);

	return (RollResult >= TargetNumber) ? ESKillCheckResult::SUCCESS : ESKillCheckResult::FAIL;
}
