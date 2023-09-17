// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystemTags.h"
#include "../RPG.h"
#include "CharacterProgression/CharacterProgressionStatics.h"

//NOT WORKING, DOESNT SEEM TO BE POSSIBLE
FGameplayTagContainer FActionSystemTags::GetActionBlockers()
{
	FGameplayTagContainer ActionBlockers;
	
	ActionBlockers.AddTag(ActionSystemTags.Status_Stunned);


	return ActionBlockers;
}

FGameplayTagContainer FActionSystemTags::GetGoverningAttributes(FGameplayTag Skill, UWorld* WorldContext)
{
	FGameplayTagContainer TagContainer;
	if (Skill.MatchesTagDepth(FActionSystemTags::Get().Skill) < 2)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get governing attribute for a skill, but did not provide a skill tag"));
		return TagContainer;
	}


	TOptional<FSkillData> SkillDataOpt = UCharacterProgressionStatics::GetSkillDataFromTag(Skill, WorldContext);
	if (SkillDataOpt.IsSet())
	{
		FSkillData& SkillData = SkillDataOpt.GetValue();
		return SkillData.GoverningAbilities;
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get Governing Attributes for a Skill that is not setup in the Skill Data Asset for this World"));
		return TagContainer;
	}
	
}

FActionSystemTags FActionSystemTags::ActionSystemTags;

