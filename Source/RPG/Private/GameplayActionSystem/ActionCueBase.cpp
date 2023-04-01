// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/ActionCueBase.h"
#include "GameplayActionSystem/GameplayActionSystemStatics.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionCue_Animation.h"
#include "GameplayActionSystem/CueTypes/ActionCue_Audio.h"
#include "GameplayActionSystem/Action.h"

UActionCueBase* UActionCueBase::CreateActionCue(FCueConfigurationData ConfigData, UAction* ParentAction, UGameplayActionComponent* InSource /*= nullptr*/, UGameplayActionComponent* InTarget /*= nullptr*/, FVector InTargetLocation /*= nullptr*/, TSubclassOf<UActionCueBase> ActionClass /*= UActionCueBase::StaticClass()*/)
{
	//if this cue is an animation and a custom class that is a child of the animation subclass was not passed in, set the class variable to the anim class
	if (ConfigData.Type == ECueType::Animation && !ActionClass->IsChildOf(UActionCue_Animation::StaticClass()))
	{
		ActionClass = UActionCue_Animation::StaticClass();
	}
	else if (ConfigData.Type == ECueType::Audio && !ActionClass->IsChildOf(UActionCue_Audio::StaticClass()))
	{
		ActionClass = UActionCue_Audio::StaticClass();
	}

	
	//create a new object and call the initialize method on it.
	//this is a virtual function, it'll be called by subclasses
	UActionCueBase* NewActionCue = NewObject<UActionCueBase>(InSource, ActionClass);
	if (NewActionCue)
	{
		NewActionCue->InitializeActionCue(ConfigData, InSource, InTarget, InTargetLocation, ParentAction);
	}
	return NewActionCue;
}

void UActionCueBase::CreateAndPlayCue(FCueConfigurationData ConfigData, UAction* ParentAction, UGameplayActionComponent* InSource /*= nullptr*/, UGameplayActionComponent* InTarget /*= nullptr*/, FVector InTargetLocation /*= FVector::ZeroVector*/, TSubclassOf<UActionCueBase> ActionClass /*= StaticClass()*/)
{
	UActionCueBase* NewCue = UActionCueBase::CreateActionCue(ConfigData, ParentAction, InSource, InTarget, InTargetLocation);

	NewCue->PlayCue();
}

void UActionCueBase::InitializeActionCue(FCueConfigurationData ConfigData, UGameplayActionComponent* InSource, UGameplayActionComponent* InTarget, FVector InTargetLocation, UAction* ParentAction)
{
	Source = InSource;
	Action = ParentAction;

	if (ConfigData.Target == ECueTarget::Target)
	{
		Target = InTarget;
	}
	else if (ConfigData.Target == ECueTarget::Source)
	{
		Target = InSource;
	}
	else if (ConfigData.Target == ECueTarget::Location)
	{
		Target = nullptr;
	}

	if (Target)
	{
		TargetLocation = Target->GetOwner()->GetActorLocation();
	}
	else
	{
		TargetLocation = InTargetLocation;
	}
	

	Type = ConfigData.Type;
	CueTarget = ConfigData.Target;
	ExecuteTime = ConfigData.ExecuteTime;
	
}

void UActionCueBase::PlayCue_Implementation()
{
	//the base implementation of this does nothing
}

float UActionCueBase::GetCueLength()
{
	return 0.f;
}
