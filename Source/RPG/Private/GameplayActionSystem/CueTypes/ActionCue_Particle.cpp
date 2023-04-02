// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/CueTypes/ActionCue_Particle.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

UFXSystemComponent* UActionCue_Particle::SpawnEmitterCascade(UWorld* WorldContext, UParticleSystem* CascadeSystem)
{
	UFXSystemComponent* SystemComp = nullptr;
	
	if (Target)
	{
		//get component to attach to - using a placeholder that justs gets the first component on the parent of the target component
		USceneComponent* AttachComponent = GetComponentToAttachTo();
		//we have a target, spawn attached
		SystemComp = Cast<UFXSystemComponent>(UGameplayStatics::SpawnEmitterAttached(CascadeSystem, AttachComponent, AttachSocket, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true));
	}
	else
	{
		SystemComp = Cast<UFXSystemComponent>(UGameplayStatics::SpawnEmitterAtLocation(WorldContext, CascadeSystem, FTransform(TargetLocation)));
	}


	return SystemComp;
}

UFXSystemComponent* UActionCue_Particle::SpawnEmitterNiagara(UWorld* WorldContext, UNiagaraSystem* NiagaraSystem)
{
	UFXSystemComponent* SystemComp = nullptr;

	if (Target)
	{
		USceneComponent* AttachComponent = GetComponentToAttachTo();
		SystemComp = Cast<UFXSystemComponent>(UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, AttachComponent, AttachSocket, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true));
	}
	else
	{
		if (bFollowMouse)
		{
			TargetLocation = GetLocationOfPlayerCursor();
			Source->OnTick.AddUniqueDynamic(this, &UActionCue_Particle::TickCue);
		}
		SystemComp = Cast<UFXSystemComponent>(UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, NiagaraSystem, TargetLocation));
	}

	return SystemComp;
}

USceneComponent* UActionCue_Particle::GetComponentToAttachTo()
{
	return Cast<USceneComponent>(Source->GetOwner()->GetComponentByClass(USceneComponent::StaticClass()));
}

FVector UActionCue_Particle::GetLocationOfPlayerCursor()
{
	APlayerController* PlayerController = Source->GetPlayerController();
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}

	FHitResult Hit;
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	return Hit.Location;
}

void UActionCue_Particle::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation, ParentAction);

	ParticleSystem = ConfigData.ParticleSystem;
	AttachSocket = ConfigData.AttachToSocket;
	bFollowMouse = ConfigData.bFollowMouseCursor;
}

float UActionCue_Particle::GetCueLength()
{
	return 0.f;
}

void UActionCue_Particle::StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState)
{
	if (ParticleSystemComponent)
	{
		ParticleSystemComponent->DeactivateImmediate();
		ParticleSystemComponent->DestroyComponent(false);
		Source->OnTick.RemoveAll(this);
	}
}

void UActionCue_Particle::TickCue(float DeltaTime)
{
	//tick particle

	FVector MouseLocation = GetLocationOfPlayerCursor();
	ParticleSystemComponent->SetWorldLocation(MouseLocation);

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
		ParticleSystemComponent = SpawnEmitterCascade(World, Cast<UParticleSystem>(ParticleSystem));
	}
	else if (Cast<UNiagaraSystem>(ParticleSystem))
	{
		ParticleSystemComponent = SpawnEmitterNiagara(World,Cast<UNiagaraSystem>(ParticleSystem));
	}

	Super::PlayCue_Implementation();

}
