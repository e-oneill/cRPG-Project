// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Misc/Optional.h"
#include "CharacterProgressionStatics.generated.h"

class UGameplayActionComponent;





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
		TArray<FGameplayTag> GoverningAbilities;
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
	
};
