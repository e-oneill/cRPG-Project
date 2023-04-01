// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/CueTypes/ActionCue_Audio.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

void UActionCue_Audio::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation, ParentAction);

	SoundCue = ConfigData.SoundCue;
	bLooping = ConfigData.bLoop;
}

float UActionCue_Audio::GetCueLength()
{
	if (!SoundCue)
	{
		return 0.f;
	}

	return SoundCue->GetDuration();
}

void UActionCue_Audio::PlayCue_Implementation()
{
	//if target is set, the sound should be attached to an actor
	if (Target)
	{
		USceneComponent* FirstSceneComponent = Cast<USceneComponent>(Target->GetOwner()->GetComponentByClass(USceneComponent::StaticClass()));
		PlayingSound = UGameplayStatics::SpawnSoundAttached(SoundCue, FirstSceneComponent);

		//UGameplayStatics::PlaySoundAtLocation(this, SoundCue, Target->GetOwner()->GetActorLocation());
	}
	//if target is not set, play the sound at location
	else
	{
		PlayingSound = UGameplayStatics::SpawnSoundAtLocation(Source, SoundCue, TargetLocation);
	}

	if (bLooping)
	{

	}

}
