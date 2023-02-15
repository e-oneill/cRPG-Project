// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/ActionConsideration.h"
#include "AC_HasPrequisiteAttributes.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UAC_HasPrequisiteAttributes : public UActionConsideration
{
	GENERATED_BODY()
	
public:
	bool CheckConsideration_Implementation(UGameplayActionComponent* Actor, FVector TargetLocation = FVector::ZeroVector, AActor* TargetActor = nullptr, UAction* Action = nullptr) override;

};
