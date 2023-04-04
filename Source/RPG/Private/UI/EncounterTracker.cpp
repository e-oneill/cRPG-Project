// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EncounterTracker.h"
#include "UI/TurnTracker.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Layout/Margin.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameState/Turn.h"
#include "GameState/Encounter.h"
#include "GameplayActionSystem/GameplayActionComponent.h"

void UEncounterTracker::OnCurrentTurnChanged(UTurn* Turn)
{
	CharacterName->SetText(FText::FromString(Turn->GetActionComponent()->GetName()));
}

void UEncounterTracker::OnEncounterEnd(AEncounter* InEncounter)
{
	TurnTrackerContainer->ClearChildren();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UEncounterTracker::SetupEncounter(AEncounter* InEncounter)
{
	//clear the turn tracker container
	if (TurnTrackerContainer->HasAnyChildren())
	{
		TurnTrackerContainer->ClearChildren();
	}
	//Set variables
	Encounter = InEncounter;
	Turns.Empty();

	//Bind to event called when the turn changes
	InEncounter->OnCurrentTurnChanged.AddUniqueDynamic(this, &UEncounterTracker::OnCurrentTurnChanged);
	InEncounter->OnEncounterEnd.AddUniqueDynamic(this, &UEncounterTracker::OnEncounterEnd);
	InEncounter->OnCharacterJoinEncounter.AddUniqueDynamic(this, &UEncounterTracker::OnCharacterJoinEncounter);
	InEncounter->OnCharacterLeftEncounter.AddUniqueDynamic(this, &UEncounterTracker::OnCharacterLeftEncounter);
	//Call method to add the turns to the turn tracker
	AddTurnsToTracker(InEncounter->GetTurns());
	//Call the event, since it doesn't fire the first time
	OnCurrentTurnChanged(InEncounter->GetCurrentTurn());
}

void UEncounterTracker::AddTurnsToTracker(TArray<UTurn*> InTurns)
{
	for (UTurn* Turn : InTurns)
	{
		AddTurnToTracker(Turn);
	}

	if (GetVisibility() != ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UEncounterTracker::AddTurnToTracker(UTurn* InTurn)
{
	//if this turn is already in the encounter, don't add it
	if (Turns.Contains(InTurn))
	{
		return;
	}
	//add turn to Encounter Array
	Turns.Add(InTurn);

	//create turn tracker widget
	UTurnTracker* TurnTracker = CreateWidget<UTurnTracker>(GetOwningPlayer(), TurnTrackerClass, (TEXT("TurnTracker_%s"), *InTurn->GetName()));
	TurnTracker->Init(InTurn);
	if (TurnTracker)
	{
		UHorizontalBoxSlot* TurnSlot = TurnTrackerContainer->AddChildToHorizontalBox(TurnTracker);
		FMargin SlotPadding;
		SlotPadding.Left = 12.f;
		SlotPadding.Right = 12.f;
		TurnSlot->SetPadding(SlotPadding);
	}
}

void UEncounterTracker::RemoveTurnFromTracker(UTurn* InTurn)
{
	if (!Turns.Contains(InTurn))
	{
		return;
	}

	Turns.Remove(InTurn);
	UWidget* WidgetToRemove = nullptr;
	for (UWidget* TurnWidget : TurnTrackerContainer->GetAllChildren())
	{
		UTurnTracker* TurnTracker = Cast<UTurnTracker>(TurnWidget);
		if (!TurnTracker)
		{
			continue;
		}

		if (TurnTracker->GetTurn() == InTurn)
		{
			WidgetToRemove = TurnWidget;
			break;
		}
	}

	if (WidgetToRemove)
	{
		TurnTrackerContainer->RemoveChild(WidgetToRemove);
		//Not certain this will get picked up by garbage collector!
	}
}
