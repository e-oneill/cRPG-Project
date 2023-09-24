// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility/ReplicatedObject.h"
#include "GameplayTagContainer.h"
#include "ActionAttribute.generated.h"

class UGameplayActionComponent;
class UActionEffect;
struct FAttributeConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeChangeSignature, UGameplayActionComponent*, OwningComponent, UActionAttribute*, Attribute);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPG_API UActionAttribute : public UReplicatedObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayTag AttributeTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	float AttributeBaseValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_AttributeValue)
	float AttributeValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	bool IgnoreMaxValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UGameplayActionComponent* ActionComponent;

	void HandleStandardChange(float val, UActionEffect* SourceEffect);

	void HandleHealthChange(float val, UActionEffect* SourceEffect);

	

	UFUNCTION()
	void OnRep_AttributeValue();

	#pragma region HandleDeathLogic
	void HandleDeath();
	#pragma endregion HandleDeathLogic

public:
	static UActionAttribute* CreateAttribute(UGameplayActionComponent* OwningComponent, FGameplayTag InAttributeTag, float InDefaultValue, bool bShouldDefaultToBaseValue = true, bool bIgnoreMaxValue = false);
	static UActionAttribute* CreateAttribute(UGameplayActionComponent* OwningComponent, FAttributeConfig AttributeConfig);

	UPROPERTY(BlueprintAssignable)
	FAttributeChangeSignature OnAttributeChanged;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ChangeAttributeValue(float val, UActionEffect* SourceEffect);

	float GetSkillCheckModifier(UGameplayActionComponent* ActingComponent);

	#pragma region GettersSetters
	float GetAttributeValue() const { return AttributeValue; }
	void SetAttributeValue(float val);
	
	
	FGameplayTag GetAttributeTag() const { return AttributeTag; }
	void SetAttributeTag(FGameplayTag val) { AttributeTag = val; }
	float GetAttributeBaseValue() const { return AttributeBaseValue; }
	void SetAttributeBaseValue(float val) { AttributeBaseValue = val; }
	UGameplayActionComponent* GetActionComponent() const { return ActionComponent; }
	void SetActionComponent(UGameplayActionComponent* val) { ActionComponent = val; }
	#pragma endregion GettersSetters
};
