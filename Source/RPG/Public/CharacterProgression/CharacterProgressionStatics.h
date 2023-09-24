// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Misc/Optional.h"
#include "CharacterProgressionStatics.generated.h"

class UGameplayActionComponent;
class UActionAttribute;
class USkillDataAsset;

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	NONE,
	Single,
	Preferred,
	Hybrid
};

USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTagContainer GoverningAbilities;
};

/**
 * 
 */
UCLASS()
class RPG_API UCharacterProgressionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	public:
	static TOptional<FSkillData> GetSkillDataFromTag(FGameplayTag Skill, UWorld* WorldContext);

		UFUNCTION(BlueprintCallable)
	static float CalculateSkillModifierForActor(FSkillData& Skill, UGameplayActionComponent* ActionComponent);

	UFUNCTION(BlueprintCallable)
	static bool CanLevelUp(UGameplayActionComponent* ToLevel);

	static bool CanLevelUp(UActionAttribute* Experience, UActionAttribute* Level, UWorld* WorldContext);

	UFUNCTION(BlueprintCallable)
	static void LevelUpCharacter(UGameplayActionComponent* ToLevel);

	UFUNCTION(BlueprintCallable)
	static USkillDataAsset* GetSkillDataAsset(UWorld* WorldContext);

	UFUNCTION(BlueprintCallable)
	static void SpendPointsOnAttribute(UGameplayActionComponent* ToSpend, UActionAttribute* AttributeToIncrease, UActionAttribute* AttributeToSpend, float AmountToSpend = 1.0f);
	
};
