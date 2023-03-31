// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/CueTypes/ActionCue_Audio.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

void UActionCue_Audio::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation);

	SoundCue = ConfigData.SoundCue;
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
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, Target->GetOwner()->GetActorLocation());
	}
	//if target is not set, play the sound at location
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, TargetLocation);
	}

	
}
