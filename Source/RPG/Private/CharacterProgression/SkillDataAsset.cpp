// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterProgression/SkillDataAsset.h"

TOptional<FSkillData> USkillDataAsset::GetSkillData(FGameplayTag Skill)
{
	if (SkillData.Find(Skill))
	{
		FSkillData* SkillEntry = SkillData.Find(Skill);
		return TOptional<FSkillData>(*SkillEntry);
	}

	return TOptional<FSkillData>();
}

int USkillDataAsset::GetXPToLevel(int Level)
{
	if (LevelUpThresholds.Num() >= Level)
	{
		return LevelUpThresholds[Level-1];
	}
	else
		return -1;
}

int const USkillDataAsset::GetMaxLevel()
{
	return LevelUpThresholds.Num();
}
