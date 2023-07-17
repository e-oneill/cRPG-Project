// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/EnvironmentalHazard.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "../RPG.h"

// Sets default values
AEnvironmentalHazard::AEnvironmentalHazard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ActionComponent = CreateDefaultSubobject<UGameplayActionComponent>(TEXT("ActionComponent"));
}

// Called when the game starts or when spawned
void AEnvironmentalHazard::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UAction> ActionClass : HazardActionClasses)
	{
		UAction* Action = NewObject<UAction>(this, ActionClass);
		Action->InitializeAction(ActionComponent);
		HazardActions.Add(Action);
	}
}

// Called every frame
void AEnvironmentalHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvironmentalHazard::Trigger_Implementation(AActor* TriggeredBy)
{
	

	UGameplayActionComponent* TriggeredByComponent = UGameplayActionComponent::GetActionComponent(TriggeredBy);
	if (!TriggeredByComponent)
	{
		return;
	}

	bTriggered = true;

	for (UAction* Action : HazardActions)
	{
		bool bExecutedAction = Action->TryExecuteAction(GetActorLocation(), TriggeredBy);
		if (!bExecutedAction)
		{
			FString Name = GetName();
			FString ActionName = Action->GetName();
			FString TargetName = TriggeredBy->GetName();
			UE_LOG(LogRPG, Warning, TEXT("%s failed to execute action %s on target %s"), *Name, *ActionName, *TargetName);
		}
	}


}

bool AEnvironmentalHazard::TryTrigger_Implementation(AActor* TriggeredBy)
{
	if (CanTrigger_Implementation(TriggeredBy))
	{
		Trigger_Implementation(TriggeredBy);
		return true;
	}
	else
		return false;
}

bool AEnvironmentalHazard::CanTrigger_Implementation(AActor* TriggeredBy)
{
	return bAllowRetrigger || !bTriggered;
}

