// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGHUD.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "../RPGPlayerController.h"
#include "Engine/Canvas.h"
#include "Interfaces/PlayerInteraction/Selectable.h"

void ARPGHUD::StartSelection_Implementation(ARPGPlayerController* PlayerController)
{
	bIsDrawing = true;
	double StartX = 0;
	double  StartY = 0;
	StartPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(PlayerController);
	StartPosition *= UWidgetLayoutLibrary::GetViewportScale(PlayerController);

	EndPosition = StartPosition;
}

void ARPGHUD::EndSelection_Implementation(ARPGPlayerController* PlayerController)
{
	
	bIsDrawing = false;
}

void ARPGHUD::DrawHUD()
{
	Super::DrawHUD();
	if (bIsDrawing)
	{
		double StartX =0;
		double StartY =0;
		
		if (StartPosition.X < EndPosition.X)
		{
			StartX = StartPosition.X;
		}
		else
		{
			StartX = EndPosition.X;
		}

		if (StartPosition.Y < EndPosition.Y)
		{
			StartY = StartPosition.Y;
		}
		else
		{
			StartY = EndPosition.Y;
		}

		DrawRect(
			MarqueeColor, 
			StartX,
			StartY,
			FMath::Abs(StartPosition.X - EndPosition.X),
			FMath::Abs(StartPosition.Y - EndPosition.Y));

		
	}

	
	
}
