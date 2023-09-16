// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystemTags.h"
#include "../RPG.h"

//NOT WORKING, DOESNT SEEM TO BE POSSIBLE
FGameplayTagContainer FActionSystemTags::GetActionBlockers()
{
	FGameplayTagContainer ActionBlockers;
	
	ActionBlockers.AddTag(ActionSystemTags.Status_Stunned);


	return ActionBlockers;
}

FGameplayTagContainer FActionSystemTags::GetGoverningAttributes(FGameplayTag Skill)
{
	FGameplayTagContainer TagContainer;
	if (Skill.MatchesTagDepth(FActionSystemTags::Get().Skill) < 2)
	{
		UE_LOG(LogRPG, Error, TEXT("Tried to get governing attribute for a skill, but did not provide a skill tag"));
		return TagContainer;
	}


	return TagContainer;
}

FActionSystemTags FActionSystemTags::ActionSystemTags;

