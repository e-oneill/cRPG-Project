// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayActionSystem/Action.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "GameplayActionSystem/ActionConsideration.h"
#include "Net/UnrealNetwork.h"
#include "GameplayActionSystem/ActionEffect.h"
#include "GameplayActionSystem/ActionCueBase.h"
#include "../RPG.h"
#include "GameplayActionSystem/ActionPlayerControlInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UAction::InitializeAction_Implementation(UGameplayActionComponent* InSource)
{
	State = EActionState::Idle;
	Source = InSource;
	if (InSource->GetOwner()->HasAuthority())
	{
		for (TSubclassOf<UActionConsideration> ConsiderationType : ConsiderationTypes)
		{
			UActionConsideration* Consideration = NewObject<UActionConsideration>(this, ConsiderationType);
			Considerations.Add(Consideration);
		}
	}
	

}

void UAction::Tick_Implementation()
{

}

bool UAction::CanPrepareAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	if (Source->GetActiveGameplayTags().HasAny(BlockedTags))
	{
		return false;
	}
	return CheckConsiderations(TargetLocation, TargetActor, EActionState::Preparing);


	/*return true;*/
}

void UAction::TryPrepareAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	if (CanPrepareAction(TargetLocation, TargetActor))
	{
		PrepareAction_Implementation(TargetLocation, TargetActor);
	}
	else
	{
		//do nothing for now
	}
}

void UAction::PrepareAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	Source->SetActiveAction(this);
	EActionState OldState = State;
	State = EActionState::Preparing;
	if (Source->GetOwner()->HasAuthority())
	{
		OnRep_State(OldState);
	}
	UGameplayActionComponent* TargetComponent = UGameplayActionComponent::GetComponentFromActor(TargetActor);
	if (bFaceTarget)
	{
		//LookAtLocation(TargetLocation, TargetActor);
	}
	PlayActionCues(ECueExecuteTime::OnPrepare, TargetComponent, TargetLocation);
}

bool UAction::CanExecuteAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	if (Source->GetActiveGameplayTags().HasAny(BlockedTags))
	{
		return false;
	}
	return CheckConsiderations(TargetLocation, TargetActor, EActionState::Executing);
}

bool UAction::TryExecuteAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	bool bCanExecuteAction = CanExecuteAction(TargetLocation, TargetActor);
	if (!bCanExecuteAction)
	{
		return false;
	}
	else
	{
		ExecuteAction(TargetLocation, TargetActor);
		return true;
	}
		
	
}

void UAction::ExecuteAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	EActionState OldState = State;
	State = EActionState::Executing;
	if (Source->GetOwner()->HasAuthority())
	{
		OnRep_State(OldState);
	}
	//if we should face the target, set the rotation
	if (bFaceTarget)
	{
		LookAtLocation(TargetLocation, TargetActor);
	}

	Source->AddGameplayTags(GrantsTags);
	UGameplayActionComponent* TargetComponent = UGameplayActionComponent::GetComponentFromActor(TargetActor);
	for (FEffectConfigurationData& ConfigData : ActionEffects)
	{
		if (TargetComponent || ConfigData.Target == EActionEffectTarget::Self || ConfigData.Target == EActionEffectTarget::AreaTarget || ConfigData.Target == EActionEffectTarget::AreaSelf)
		{
			UActionEffect* NewEffect = UActionEffect::CreateActionEffect(ConfigData, Source, TargetComponent, TargetLocation);
			NewEffect->TriggerEffect();
		}
		
	}
	//play all the relevant action cues
	PlayActionCues(ECueExecuteTime::OnExecute, TargetComponent, TargetLocation);
	
	//if none of the action cues started a timer to move to the next stage, complete action
	if (!bNoAutoStop && !GetWorld()->GetTimerManager().IsTimerActive(ActionCueTimer))
	{
		CompleteAction(TargetLocation, TargetActor);
	}
	


}

void UAction::CompleteAction_Implementation(FVector TargetLocation /*= FVector::ZeroVector*/, AActor* TargetActor /*= nullptr*/)
{
	
	EActionState OldState = State;
	State = EActionState::Complete;
	for (FGameplayTag GrantTag : GrantsTags)
	{
		Source->RemoveGameplayTag(GrantTag);
	}
	if (Source->GetOwner()->HasAuthority())
	{
		OnRep_State(OldState);
	}

	

}

void UAction::ResetAction_Implementation()
{
	EActionState OldState = State;
	State = EActionState::Idle;
	if (Source->GetOwner()->HasAuthority())
	{
		OnRep_State(OldState);
	}
}

void UAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAction, Source);
	DOREPLIFETIME(UAction, State);
}

void UAction::OnRep_State(EActionState& OldState)
{
	switch (State)
	{
		case EActionState::Idle:
			OnActionCancel.Broadcast(this, State, OldState);
		case EActionState::Preparing:
			OnActionPrepare.Broadcast(this, State, OldState);
			break;
		case EActionState::Executing:
			OnActionExecute.Broadcast(this, State, OldState);
			break;
		case EActionState::Complete:
			OnActionComplete.Broadcast(this, State, OldState);
			break;
		case EActionState::Canceling:
			OnActionCancel.Broadcast(this, State, OldState);
			break;
	}

}

void UAction::PostInitProperties()
{
	Super::PostInitProperties();

	SetupActionData();
}

void UAction::SetupActionData()
{
	//do nothing
}

void UAction::LookAtLocation(FVector TargetLocation, AActor* TargetActor)
{
	//for now, this just sets the rotation, this is likely too quick.
	//expand later with more of a turn.
	FVector LookLocation = TargetLocation;
	if (TargetActor)
	{
		LookLocation = TargetActor->GetActorLocation();
	}
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Source->GetOwner()->GetActorLocation(), LookLocation);
	LookAtRotation.Pitch = Source->GetOwner()->GetActorRotation().Pitch;
	LookAtRotation.Roll = Source->GetOwner()->GetActorRotation().Roll;
	Source->GetOwner()->SetActorRotation(LookAtRotation);
}

void UAction::PlayActionCues(ECueExecuteTime ExecuteTime, UGameplayActionComponent* TargetComponent, FVector TargetLocation)
{
	for (int i = 0; i < ActionCues.Num(); i++)
	{
		FCueConfigurationData& CueConfig = ActionCues[i];
		if (CueConfig.ExecuteTime == ExecuteTime)
		{
			
			
			if (CueConfig.Relevancy == ECueRelevancy::NetMulticast)
			{
				NetMulticast_PlayCue(CueConfig, TargetComponent, TargetLocation);
			}
			else
			{	
				//get the interface we can use to direct a call to the player controller
				//this is necessary because an object that inherits from character has its net owner overwritten to be its controller
				//in our game player controlled characters have an AIController
				
				TScriptInterface<IActionPlayerControlInterface> PCScriptInterface = Source->GetPlayerControlInterface();
				
				if (!PCScriptInterface.GetObject())
				{
					//returned interface is probably nullptr, meaning that the component is likely not player controlled
					continue;
				}
				//As the Interface method is BlueprintNativeEvent we must call it through the interface static wrapper
				IActionPlayerControlInterface::Execute_Client_PlayActionCue(Cast<UObject>(PCScriptInterface.GetObject()), this, CueConfig, TargetComponent, TargetLocation);
			}
			if (CueConfig.bHaltActionUntilComplete)
			{
				float AnimLength = CueConfig.AnimationMontage->GetPlayLength();

				//Target Component may be nullptr, so do a boolean check to ensure it exists
				AActor* TargetActor = nullptr;
				if (TargetComponent)
					TargetActor = TargetComponent->GetOwner();

				FTimerDelegate FuncDelegate = GetDelegateForNextState(TargetLocation, TargetActor, ExecuteTime);
				GetWorld()->GetTimerManager().SetTimer(ActionCueTimer, FuncDelegate, AnimLength, false);
			}
		}
	}
}

void UAction::NetMulticast_PlayCue_Implementation(const FCueConfigurationData& CueConfig, UGameplayActionComponent* TargetComponent, const FVector& TargetLocation)
{
	UE_LOG(LogRPG, Log, TEXT("Calling NetMulticast"));
	UActionCueBase::CreateAndPlayCue(CueConfig, this, Source, TargetComponent, TargetLocation);
}

FTimerDelegate UAction::GetDelegateForNextState(FVector TargetLocation, AActor* TargetActor, ECueExecuteTime ExecuteTime)
{
	switch (ExecuteTime)
	{
	case ECueExecuteTime::OnStartPrepare:

	case ECueExecuteTime::OnPrepare:

	case ECueExecuteTime::OnEndPrepare:
		return FTimerDelegate::CreateUObject(this, &UAction::ExecuteAction, TargetLocation, TargetActor);
		break;
	case ECueExecuteTime::OnStartExecute:

	case ECueExecuteTime::OnExecute:

	case ECueExecuteTime::OnEndExecute:
		
	default:
		return FTimerDelegate::CreateUObject(this, &UAction::CompleteAction, TargetLocation, TargetActor);
		break;

	}
	
}

bool UAction::CheckConsiderations(FVector TargetLocation, AActor* TargetActor, EActionState StateToCheck)
{
	for (int i = 0; i < Considerations.Num(); i++)
	{
		UActionConsideration* Consideration = Considerations[i];
		//check if this consideration should not be applied
		if (
			(StateToCheck == EActionState::Preparing && Consideration->GetConsiderationAppliesTo() == EConsiderationApplication::Execute) ||
			(StateToCheck == EActionState::Executing && Consideration->GetConsiderationAppliesTo() == EConsiderationApplication::Prepare)
			)
		{
			continue;
		}
		bool CheckConsideration = Consideration->CheckConsideration(Source, TargetLocation, TargetActor, this);
		if (!CheckConsideration)
		{
			return CheckConsideration;
		}
	}
	return true;
}

