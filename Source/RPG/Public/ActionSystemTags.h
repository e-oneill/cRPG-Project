#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

struct RPG_API FActionSystemTags : public FGameplayTagNativeAdder
{
#pragma region TagContainers
	
#pragma endregion

#pragma region Attributes
	FGameplayTag Attribute;
	FGameplayTag Attr_Health;
	//the distance a character can move in a turn
	FGameplayTag Attr_Movement;
	FGameplayTag Attr_ActionPoints;
#pragma endregion Attributes

#pragma region Actions
	FGameplayTag Action;

	FGameplayTag Action_Move;
	FGameplayTag Action_Attack;
#pragma endregion Actions

#pragma region Statuses
	FGameplayTag Status;

	FGameplayTag Status_Stunned;
#pragma region

	FORCEINLINE static const FActionSystemTags& Get() { return ActionSystemTags; }

	static FGameplayTagContainer GetActionBlockers();

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		Action = Manager.AddNativeGameplayTag(TEXT("Action"));
		Action_Move = Manager.AddNativeGameplayTag(TEXT("Action.Move"));
		Action_Attack = Manager.AddNativeGameplayTag(TEXT("Action.Attack"));

		Status = Manager.AddNativeGameplayTag(TEXT("Status"));
		Status_Stunned = Manager.AddNativeGameplayTag(TEXT("Status.Stunned"));

		Attribute = Manager.AddNativeGameplayTag(TEXT("Attribute"));
		Attr_Health = Manager.AddNativeGameplayTag(TEXT("Attribute.Health"));
		Attr_Movement = Manager.AddNativeGameplayTag(TEXT("Attribute.Movement"));
		Attr_ActionPoints = Manager.AddNativeGameplayTag(TEXT("Attribute.ActionPoints"));
	}

private:
	static FActionSystemTags ActionSystemTags;

};