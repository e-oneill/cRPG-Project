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
