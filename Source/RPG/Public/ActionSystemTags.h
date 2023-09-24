#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

struct RPG_API FActionSystemTags : public FGameplayTagNativeAdder
{
#pragma region Tags
#pragma region Attributes
	FGameplayTag Attribute;
	FGameplayTag Attr_Health;
	FGameplayTag Attr_PhysicalArmour;
	FGameplayTag Attr_MagicalArmour;
	//the distance a character can move in a turn
	FGameplayTag Attr_Movement;
	FGameplayTag Attr_ActionPoints;

	FGameplayTag Attr_Level;
	FGameplayTag Attr_Experience;
	FGameplayTag Attr_ExperienceGranted;

	//ability scores
	FGameplayTag Might;
	FGameplayTag Agility;
	FGameplayTag Vitality;
	FGameplayTag Intellect;
	FGameplayTag Intuition;

#pragma region Skills
	FGameplayTag Skill;
	FGameplayTag Skill_Thieving;
#pragma endregion Skills

#pragma region AttributePoints
	FGameplayTag AbilityPoints;
	FGameplayTag SkillPoints;
#pragma endregion AttributePoints

#pragma endregion Attributes





#pragma region DamageTypes
	FGameplayTag Dmg_Physical;
	FGameplayTag Dmg_Magical;
#pragma endregion DamageTypes

#pragma region Actions
	FGameplayTag Action;

	FGameplayTag Action_Move;
	FGameplayTag Action_Attack;
#pragma endregion Actions

#pragma region Factions
	FGameplayTag Player_Faction;
#pragma endregion Factions

#pragma region Statuses
	FGameplayTag Status;

	FGameplayTag Status_Stunned;
#pragma endregion Statuses

#pragma endregion Tags


	FORCEINLINE static const FActionSystemTags& Get() { return ActionSystemTags; }

	static FGameplayTagContainer GetActionBlockers();

public:
	static FGameplayTagContainer GetGoverningAttributes(FGameplayTag Skill, UWorld* WorldContext);

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

		Might = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Might"));
		Agility = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Agility"));
		Vitality = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Vitality"));
		Intellect = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Intellect"));
		Intuition = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Intuition"));

		Skill = Manager.AddNativeGameplayTag(TEXT("Attribute.Skill"));
		Skill_Thieving = Manager.AddNativeGameplayTag(TEXT("Attribute.Skill.Thieving"));

		AbilityPoints = Manager.AddNativeGameplayTag(TEXT("Attribute.Ability.Points"));
		SkillPoints = Manager.AddNativeGameplayTag(TEXT("Attribute.Skill.Points"));
		
		Attr_Level = Manager.AddNativeGameplayTag(TEXT("Attribute.Level"));
		Attr_Experience = Manager.AddNativeGameplayTag(TEXT("Attribute.Experience"));
		Attr_ExperienceGranted = Manager.AddNativeGameplayTag(TEXT("Attribute.ExperienceGranted"));

		Attr_Health = Manager.AddNativeGameplayTag(TEXT("Attribute.Health"));
		Attr_PhysicalArmour = Manager.AddNativeGameplayTag(TEXT("Attribute.PhysicalArmour"));
		Attr_MagicalArmour = Manager.AddNativeGameplayTag(TEXT("Attribute.MagicalArmour"));

		Dmg_Physical = Manager.AddNativeGameplayTag(TEXT("Damage.Physical"));
		Dmg_Magical = Manager.AddNativeGameplayTag(TEXT("Damage.Magical"));

		Attr_Movement = Manager.AddNativeGameplayTag(TEXT("Attribute.Movement"));
		Attr_ActionPoints = Manager.AddNativeGameplayTag(TEXT("Attribute.ActionPoints"));

		Player_Faction = Manager.AddNativeGameplayTag(TEXT("Faction.Player"));


	}

private:
	static FActionSystemTags ActionSystemTags;

};