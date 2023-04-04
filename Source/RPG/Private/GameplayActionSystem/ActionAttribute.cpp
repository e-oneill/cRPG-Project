// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionAttribute.h"
#include "Net/UnrealNetwork.h"
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

void UActionAttribute::SetAttributeValue(float val)
{
	AttributeValue = FMath::Clamp(val, 0.f, AttributeBaseValue); 
	
	OnAttributeChanged.Broadcast(ActionComponent, this);

	if (AttributeTag == FActionSystemTags::Get().Attr_Health && AttributeValue <= 0)
	{
		//if the attribute is Health, we should run low health logic at zero hp
		HandleDeath();
	}
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