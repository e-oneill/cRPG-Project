// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/CueTypes/ActionCue_Particle.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"

UFXSystemComponent* UActionCue_Particle::SpawnEmitterCascade(UParticleSystem* CascadeSystem)
{
	return nullptr;
}

UFXSystemComponent* UActionCue_Particle::SpawnEmitterNiagara(UNiagaraSystem* NiagaraSystem)
{
	return nullptr;
}

void UActionCue_Particle::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation, ParentAction);

	ParticleSystem = ConfigData.ParticleSystem;
	AttachSocket = ConfigData.AttachToSocket;
}

float UActionCue_Particle::GetCueLength()
{
	return 0.f;
}

void UActionCue_Particle::StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState)
{
	
}

void UActionCue_Particle::PlayCue_Implementation()
{
	UWorld* World = Source->GetWorld();
	if (!World)
	{
		return;
	}

	if (Cast<UParticleSystem>(ParticleSystem))
	{
		ParticleSystemComponent = SpawnEmitterCascade(Cast<UParticleSystem>(ParticleSystem));
	}
	else if (Cast<UNiagaraSystem>(ParticleSystem))
	{
		ParticleSystemComponent = SpawnEmitterNiagara(Cast<UNiagaraSystem>(ParticleSystem));
	}

	Super::PlayCue_Implementation();

}
