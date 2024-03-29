// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PlayerInteraction/Interactable.h"
#include "GameplayTagContainer.h"
#include "InteractableActor.generated.h"

USTRUCT(BlueprintType)
struct FInteractableAttributeRequirement
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, meta = (Categories = "Attribute."))
	FGameplayTag Attribute;

	UPROPERTY(EditAnywhere)
	float ValueRequired; 

	//if this bool is set to true, the interactable will deduce the required amount from the value of this attribute
	UPROPERTY(EditAnywhere)
	bool bDeductAsCost;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bDeductAsCost", EditConditionHides))
	bool bPayCostOnlyFirstTime;

};

UCLASS()
class RPG_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	bool bInteracted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InteractionDistance = 300.f;

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "{Attribute}: {ValueRequired}"))
	TArray<FInteractableAttributeRequirement> AttributeRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSkillCheckToInteract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Attribute.Skill.", EditCondition = "bSkillCheckToInteract", EditConditionHides))
	FGameplayTag SkillToCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bSkillCheckToInteract", EditConditionHides))
	int32 TargetNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bSkillCheckToInteract", EditConditionHides))
	bool bMustRetakeCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bSkillCheckToInteract", EditConditionHides, ToolTip = "The target number will increment by this amount each time you fail."))
	int32 RampDifficulty;

	UPROPERTY(BlueprintReadWrite)
	bool bSkillCheckSucceeded = false;

public:	
	bool GetInteracted() const {return bInteracted;}
	void SetInteracted(bool bInInteracted) {bInteracted = bInInteracted;}


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanInteract_Implementation(AActor* Interactor) override;

	bool Interact_Implementation(AActor* Interactor) override;

	bool SkillCheckSucceeded() const { return bSkillCheckSucceeded; }
	void SkillCheckSucceeded(bool val) { bSkillCheckSucceeded = val; }
};
