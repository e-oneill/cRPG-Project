// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/CueTypes/ActionCue_Audio.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/Action.h"
#include "Components/AudioComponent.h"

void UActionCue_Audio::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction)
{
	Super::InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation, ParentAction);

	SoundCue = ConfigData.SoundCue;
	bLooping = ConfigData.bLoop;
	EndOn = ConfigData.EndOn;
	b2DSound = ConfigData.b2DSound;
}

float UActionCue_Audio::GetCueLength()
{
	if (!SoundCue)
	{
		return 0.f;
	}

	return SoundCue->GetDuration();
}

void UActionCue_Audio::StopCuePlayback(UAction* InAction, EActionState State, EActionState OldState)
{
	if (PlayingSound)
	{
		PlayingSound->Stop();
	}
}

void UActionCue_Audio::PlayCue_Implementation()
{
	//if target is set, the sound should be attached to an actor
	if (b2DSound)
	{
		PlayingSound = UGameplayStatics::SpawnSound2D(Source, SoundCue);
	}
	else if (Target)
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
		
		switch (EndOn)
		{
		case ECueExecuteTime::OnStartPrepare:
		case ECueExecuteTime::OnPrepare:
			Action->OnActionPrepare.AddUniqueDynamic(this, &UActionCue_Audio::StopCuePlayback);
			break;
		case ECueExecuteTime::OnEndPrepare:
		case ECueExecuteTime::OnStartExecute:
		case ECueExecuteTime::OnExecute:
			Action->OnActionExecute.AddUniqueDynamic(this, &UActionCue_Audio::StopCuePlayback);
			break;
		case ECueExecuteTime::OnEndExecute:
			Action->OnActionComplete.AddUniqueDynamic(this, &UActionCue_Audio::StopCuePlayback);
			
			break;
		default:
			break;

		}

		Action->OnActionCancel.AddUniqueDynamic(this, &UActionCue_Audio::StopCuePlayback);
	}

}
