// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "ActionCue_Particle.generated.h"

class UFXSystemComponent;
class UFXSystemAsset;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class RPG_API UActionCue_Particle : public UActionCueBase
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadOnly)
	UFXSystemAsset* ParticleSystem;

	UPROPERTY(BlueprintReadOnly)
	UFXSystemComponent* ParticleSystemComponent; 

	UPROPERTY(BlueprintReadOnly)
	FName AttachSocket;

	bool bFollowMouse;

	UFXSystemComponent* SpawnEmitterCascade(UWorld* WorldContext, UParticleSystem* CascadeSystem);

	UFXSystemComponent* SpawnEmitterNiagara(UWorld* WorldContext, UNiagaraSystem* NiagaraSystem);

	USceneComponent* GetComponentToAttachTo();

	FVector GetLocationOfPlayerCursor();

public:
	void InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction) override;


	void PlayCue_Implementation() override;


	float GetCueLength() override;


	void StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState) override;


	void TickCue(float DeltaTime) override;

};
