// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "EnvironmentalTriggerActor.generated.h"


class UBoxComponent;
class UDecalComponent;
class ITriggerable;

UCLASS()
class RPG_API AEnvironmentalTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentalTriggerActor();

	//virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger")
	bool bIsVisible;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger")
	bool bShowDecal = true;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger")
	bool bAutoRetrigger = false;

	//Will cause the trigger to keep looping, if false will just retrigger once
	UPROPERTY(EditAnywhere, Category = "Environmental Trigger")
	bool bLooping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Trigger", meta = (Categories="Faction."))
	FGameplayTagContainer IgnoreFaction;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger", meta = (EditCondition = "bAutoRetrigger"))
	bool bStartDelayAfterOverlapEnd = false;

	UPROPERTY(EditAnywhere, Category = "Environmental Trigger", meta = (EditCondition = "bAutoRetrigger"))
	float RetriggerDelay;

	FTimerHandle RetriggerTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environmental Trigger")
	UBoxComponent* TriggerCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environmental Trigger")
	UDecalComponent* SpottedDecal;

	UFUNCTION()
	void OnColliderOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnColliderOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Environmental Trigger")
	//TScriptInterface<ITriggerable> Triggers;

	AActor* Triggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environmental Trigger")
	bool bAllowParallelTriggers;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Environmental Trigger")
	bool bTriggered = false;

	UFUNCTION()
	bool ValidateTriggers();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure, Category = "Environmental Trigger")
	bool GetIsVisible() const {return bIsVisible;}

	UFUNCTION(BlueprintCallable, Category = "Environmental Trigger")
	void SetIsVisible(bool bInIsVisible);

	UFUNCTION(BlueprintCallable, Category = "Environmental Trigger")
	void TriggerActor(UGameplayActionComponent* TriggeringActor);

	UFUNCTION()
	void SetTriggerTimer(UGameplayActionComponent* TriggeringActor, bool bClearExisting = true);

	UFUNCTION(BlueprintImplementableEvent)
	void OnVisibilityChanged(bool NewVisibility);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTriggered(UGameplayActionComponent* TriggeringActor);

	

};
