// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagsManager.h"
#include "CharacterProgressionStatics.h"
#include "SkillDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpReward
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag AttributeToReward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AmountToReward;
};

/**
 * 
 */
UCLASS()
class RPG_API USkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

	protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FSkillData> SkillData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<int> LevelUpThresholds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FLevelUpReward> LevelUpRewards;

	public:
	TOptional<FSkillData> GetSkillData(FGameplayTag Skill);

	UFUNCTION(BlueprintCallable)
	int GetXPToLevel(int Level);

	UFUNCTION(BlueprintPure)
	int const GetMaxLevel();

	UFUNCTION(BlueprintPure)
	TArray<FLevelUpReward> GetLevelUpRewards() const {return LevelUpRewards;}

	
};
