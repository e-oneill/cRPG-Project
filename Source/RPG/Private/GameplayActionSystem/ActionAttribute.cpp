// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionAttribute.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/ActionEffect.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "ActionSystemTags.h"
#include "RPGGameStatics.h"



void UActionAttribute::OnRep_AttributeValue()
{
	//if this is firing, the attribute value has changed. Fire the OnChangeEvent
	OnAttributeChanged.Broadcast(ActionComponent, this);
}



UActionAttribute* UActionAttribute::CreateAttribute(UGameplayActionComponent* OwningComponent,FGameplayTag InAttributeTag, float InDefaultValue, bool bShouldDefaultToBaseValue /* = true*/)
{
	UActionAttribute* NewAttribute = NewObject<UActionAttribute>(OwningComponent, FName(InAttributeTag.ToString()));
	NewAttribute->SetActionComponent(OwningComponent);
	NewAttribute->SetAttributeTag(InAttributeTag);
	NewAttribute->SetAttributeBaseValue(InDefaultValue);
	if (bShouldDefaultToBaseValue)
	{
		NewAttribute->SetAttributeValue(InDefaultValue);
	}
	else
	{
		NewAttribute->SetAttributeValue(0.f);
	}
	return NewAttribute;
}

void UActionAttribute::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UActionAttribute, AttributeTag);
	DOREPLIFETIME(UActionAttribute, AttributeBaseValue);
	DOREPLIFETIME(UActionAttribute, AttributeValue);
	DOREPLIFETIME(UActionAttribute, ActionComponent);

}

void UActionAttribute::ChangeAttributeValue(float val, UActionEffect* SourceEffect)
{
	if (AttributeTag == FActionSystemTags::Get().Attr_Health)
	{
		HandleHealthChange(val, SourceEffect);
	}
	else
	{
		HandleStandardChange(val, SourceEffect);
		
	}
	//OnAttributeChanged.Broadcast(ActionComponent, this);
}

void UActionAttribute::HandleStandardChange(float val, UActionEffect* SourceEffect)
{
	float NewAttributeValue = AttributeValue - val;

	AttributeValue = FMath::Clamp(NewAttributeValue, 0.f, AttributeBaseValue);
	OnAttributeChanged.Broadcast(ActionComponent, this);
}

void UActionAttribute::HandleHealthChange(float val, UActionEffect* SourceEffect)
{
	if (!ActionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to do health damage to an actor with no action component!"));
		return;
	}

	UActionAttribute* PhysicalArmour = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_PhysicalArmour);
	UActionAttribute* MagicalArmour = ActionComponent->GetAttributeByTag(FActionSystemTags::Get().Attr_MagicalArmour);
	FGameplayTag SourceDamageType = SourceEffect->GetDamageType();

	if (val > 0 && PhysicalArmour && SourceDamageType.MatchesTag(FActionSystemTags::Get().Dmg_Physical))
	{
		if (PhysicalArmour->GetAttributeValue() < val)
		{
			float ArmourValue = PhysicalArmour->GetAttributeValue();
			PhysicalArmour->ChangeAttributeValue(val, SourceEffect);
			val -= ArmourValue;
		}
		else
		{
			PhysicalArmour->ChangeAttributeValue(val, SourceEffect);
			return;
		}

	}
	else if (val > 0 && MagicalArmour && SourceDamageType.MatchesTag(FActionSystemTags::Get().Dmg_Magical))
	{
		if (MagicalArmour->GetAttributeValue() < val)
		{
			float ArmourValue = MagicalArmour->GetAttributeValue();
			MagicalArmour->ChangeAttributeValue(val, SourceEffect);
			val -= ArmourValue;
		}
		else
		{
			MagicalArmour->ChangeAttributeValue(val, SourceEffect);
			return;
		}
	}

	HandleStandardChange(val, SourceEffect);

	if (AttributeValue <= 0)
	{
		//if the attribute is Health, we should run low health logic at zero hp
		HandleDeath();
	}
}

void UActionAttribute::SetAttributeValue(float val)
{
	AttributeValue = FMath::Clamp(val, 0.f, AttributeBaseValue);
	OnAttributeChanged.Broadcast(ActionComponent, this);
}



void UActionAttribute::HandleDeath()
{
	bool bIsPlayerControlled = (URPGGameStatics::GetActionComponentPlayerController(ActionComponent) != nullptr);

	if (bIsPlayerControlled)
	{
		ActionComponent->IsUnconscious(true);	
	}
	else
	{
		ActionComponent->IsDead(true);
	}
}