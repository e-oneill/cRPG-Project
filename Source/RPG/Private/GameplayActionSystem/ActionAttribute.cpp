// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionAttribute.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/GameplayActionComponent.h"



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
