// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/EnvironmentalTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "Interfaces/Triggerable.h"
#include "../RPG.h"
#include "TimerManager.h"

// Sets default values
AEnvironmentalTriggerActor::AEnvironmentalTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Collider"));
	TriggerCollider->InitBoxExtent(FVector(200, 200, 100));

	SpottedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Spotted Decal"));
	SpottedDecal->SetupAttachment(TriggerCollider);
	SpottedDecal->DecalSize =FVector(50, 200, 200);
	SpottedDecal->SetRelativeRotation(FRotator(90, 0 , 0));
	SpottedDecal->SetRelativeLocation(FVector(0,0, -100));

	TriggerCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnvironmentalTriggerActor::OnColliderOverlap);
	TriggerCollider->OnComponentEndOverlap.AddUniqueDynamic(this, &AEnvironmentalTriggerActor::OnColliderOverlapEnd);
	Triggers = nullptr;
	bIsVisible = false;

}

void AEnvironmentalTriggerActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AEnvironmentalTriggerActor, Triggers))
	{
		ValidateTriggers();
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AEnvironmentalTriggerActor, bShowDecal))
	{
		if (bShowDecal)
		{
			SpottedDecal->SetVisibility(true, true);
		}
		else
		{
			SpottedDecal->SetVisibility(false, true);
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

// Called when the game starts or when spawned
void AEnvironmentalTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsVisible)
	{
		SpottedDecal->SetVisibility(false, true);
	}

}

void AEnvironmentalTriggerActor::OnColliderOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameplayActionComponent* ActionComp = Cast<UGameplayActionComponent>(OtherActor->GetComponentByClass(UGameplayActionComponent::StaticClass()));
	if (ActionComp && (bAllowParallelTriggers || !bTriggered))
	{
		bTriggered = true;
		TriggerActor(ActionComp);
	}
}

void AEnvironmentalTriggerActor::OnColliderOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TSet<AActor*> Overlapping;
	TriggerCollider->GetOverlappingActors(Overlapping);
	TSet<AActor*> OverlappingActionComponentActors;

	for (AActor* Overlapped : Overlapping)
	{
		if (Overlapped->GetComponentByClass(UGameplayActionComponent::StaticClass()))
		{
			OverlappingActionComponentActors.Add(Overlapped);
		}
	}


	//no one is currently overlapping
	if (bTriggered && OverlappingActionComponentActors.Num() == 0)
	{
		bTriggered = false;
		if (bAutoRetrigger && bStartDelayAfterOverlapEnd)
		{
			SetTriggerTimer(UGameplayActionComponent::GetActionComponent(OtherActor));
		}
	}
}

bool AEnvironmentalTriggerActor::ValidateTriggers()
{
	if (Triggers && !Cast<ITriggerable>(Triggers))
	{
		FString MyName = GetName();
		FString OtherName = Triggers->GetName();
		UE_LOG(LogRPG, Error, TEXT("Environmental Trigger %s has been assigned to trigger %s, but it does not implement the Triggerable interface."), *MyName, *OtherName);
		return false;
	}

	return Triggers != nullptr;
}

// Called every frame
void AEnvironmentalTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvironmentalTriggerActor::SetIsVisible(bool bInIsVisible)
{
	bIsVisible = bInIsVisible;

	if (bIsVisible)
	{
		if (bShowDecal)
			SpottedDecal->SetVisibility(true, true);
	}
	else
	{
		if (bShowDecal)
			SpottedDecal->SetVisibility(false, true);
	}

	OnVisibilityChanged(bIsVisible);
}

void AEnvironmentalTriggerActor::TriggerActor(UGameplayActionComponent* TriggeringActor)
{
	if (!bIsVisible)
	{
		SetIsVisible(true);
	}

	OnTriggered(TriggeringActor);

	if (!ValidateTriggers())
	{
		return;
	}

	ITriggerable::Execute_TryTrigger(Triggers, this);
	
	if ((bTriggered || bLooping) && bAutoRetrigger && !bStartDelayAfterOverlapEnd)
	{
		SetTriggerTimer(TriggeringActor);
	}

}

void AEnvironmentalTriggerActor::SetTriggerTimer(UGameplayActionComponent* TriggeringActor, bool bClearExisting /*= true*/)
{
	auto TimerDelegate = FTimerDelegate::CreateLambda([this, TriggeringActor]()
		{
			TriggerActor(TriggeringActor);
		});

	if (bClearExisting)
	{
		GetWorld()->GetTimerManager().ClearTimer(RetriggerTimer);
	}

	GetWorld()->GetTimerManager().SetTimer(RetriggerTimer, TimerDelegate, RetriggerDelay, false);
}

