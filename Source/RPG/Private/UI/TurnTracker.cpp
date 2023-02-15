// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TurnTracker.h"
#include "GameState/Turn.h"

void UTurnTracker::Init_Implementation(UTurn* InTurn)
{
	Turn = InTurn;

	Turn->OnTurnStart.AddDynamic(this, &UTurnTracker::OnTurnStart);
	Turn->OnTurnEnd.AddDynamic(this, &UTurnTracker::OnTurnEnd);
}

void UTurnTracker::OnTurnStart_Implementation(UTurn* InTurn)
{
	bIsActive = true;
}

void UTurnTracker::OnTurnEnd_Implementation(UTurn* InTurn)
{
	bIsActive = false;
}

