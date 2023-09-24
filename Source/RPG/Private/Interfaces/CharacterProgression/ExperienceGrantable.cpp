// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/CharacterProgression/ExperienceGrantable.h"
#include "ActionSystemTags.h"
#include "GameplayActionSystem/ActionAttribute.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

// Add default functionality here for any IExperienceGrantable functions that are not pure virtual.
int IExperienceGrantable::GrantExperience_Implementation(UGameplayActionComponent* Grantee)
{
		if (IExperienceGrantable::Execute_CanGrantExperience(_getUObject(), Grantee))
		{
			UActionAttribute* GranteeXP = Grantee->GetAttributeByTag(FActionSystemTags::Get().Attr_Experience);
			float XPToGrant = IExperienceGrantable::Execute_GetExperienceToGrant(_getUObject());

			if (GranteeXP && XPToGrant != -1.f)
			{
				GranteeXP->ChangeAttributeValue(-1*XPToGrant, nullptr);
				return XPToGrant;
			}
		}
	//if we have not returned, something went wrong, return -1 to communicate this
	return -1;
}


float IExperienceGrantable::GetExperienceToGrant_Implementation()
{
	AActor* OuterActor = Cast<AActor>(this);
	if (!OuterActor)
	{
		return -1.f;
	}
	UGameplayActionComponent* Granter = Cast<UGameplayActionComponent>(OuterActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (!Granter)
	{
		return -1.f;
	}
	UActionAttribute* GranterXPGiven = Granter->GetAttributeByTag(FActionSystemTags::Get().Attr_ExperienceGranted);
	if (GranterXPGiven)
	{
		return GranterXPGiven->GetAttributeValue();
	}
	else
	{
		return -1.f;
	}
}

bool IExperienceGrantable::CanGrantExperience_Implementation(UGameplayActionComponent* Grantee)
{
	AActor* OuterActor = Cast<AActor>(this);
	if (!OuterActor)
	{
		return false;
	}
	UGameplayActionComponent* Granter = Cast<UGameplayActionComponent>(OuterActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	return Granter && (Granter->GetFaction() != Grantee->GetFaction() && Grantee->GetFaction() == FActionSystemTags::Get().Player_Faction);
}