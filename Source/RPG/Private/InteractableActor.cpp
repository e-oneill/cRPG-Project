// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AInteractableActor::CanInteract_Implementation(AActor* Interactor)
{
	bool bInRange = FVector::Dist(Interactor->GetActorLocation(), GetActorLocation()) <= InteractionDistance;

	UGameplayActionComponent* ActionComponent = Cast<UGameplayActionComponent>(Interactor->GetComponentByClass(UGameplayActionComponent::StaticClass()));

	if (!ActionComponent && AttributeRequirements.Num() > 0)
	{
		return false;
	}

	bool bHasAttribute = true;
	for (FInteractableAttributeRequirement& Requirement : AttributeRequirements)
	{
		if (Requirement.bPayCostOnlyFirstTime && bInteracted)
		{
			continue;
		}

		UActionAttribute* Attribute = ActionComponent->GetAttributeByTag(Requirement.Attribute);
		if (!Attribute || Attribute->GetAttributeValue() < Requirement.ValueRequired)
		{
			bHasAttribute = false;
			break;
		}
	}

	return bInRange && bHasAttribute;
}

bool AInteractableActor::Interact_Implementation(AActor* Interactor)
{
	UGameplayActionComponent* ActionComponent = Cast<UGameplayActionComponent>(Interactor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	for (FInteractableAttributeRequirement& Requirement : AttributeRequirements)
	{
		if (Requirement.bDeductAsCost == false || (Requirement.bPayCostOnlyFirstTime && bInteracted))
		{
			continue;
		}

		UActionAttribute* Attribute = ActionComponent->GetAttributeByTag(Requirement.Attribute);
		if (Attribute)
		{
			Attribute->ChangeAttributeValue(Requirement.ValueRequired, nullptr);
		}

	}

	bInteracted = true;

	return IInteractable::Execute_CanInteract(this, Interactor);
}

