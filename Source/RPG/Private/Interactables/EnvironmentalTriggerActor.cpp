// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/EnvironmentalTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

// Sets default values
AEnvironmentalTriggerActor::AEnvironmentalTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Collider"));
	TriggerCollider->InitBoxExtent(FVector(200, 200, 100S1p));

	SpottedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Spotted Decal"));
	SpottedDecal->SetupAttachment(TriggerCollider);
	SpottedDecal->DecalSize =FVector(50, 200, 200);
	SpottedDecal->SetRelativeRotation(FRotator(90, 0 , 0));
	SpottedDecal->SetRelativeLocation(FVector(0,0, -100));

	TriggerCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnvironmentalTriggerActor::OnColliderOverlap);
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
	if (ActionComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapped with action component actor"));

		TriggerActor(ActionComp);
	}
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
		SpottedDecal->SetVisibility(true, true);
	}
}

void AEnvironmentalTriggerActor::TriggerActor(UGameplayActionComponent* TriggeringActor)
{
	if (!bIsVisible)
	{
		SetIsVisible(true);
	}
}

