// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUD.generated.h"

class ARPGPlayerController;

/**
 * 
 */
UCLASS()
class RPG_API ARPGHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartSelection(ARPGPlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndSelection(ARPGPlayerController* PlayerController);

	bool IsDrawing() const { return bIsDrawing; }
	void SetIsDrawing(bool val) { bIsDrawing = val; }

	void DrawHUD() override;

	TArray<AActor*> GetHighlightedActors() const { return HighlightedActors; }
	void SetHighlightedActors(TArray<AActor*> val) { HighlightedActors = val; }
	void EmptyHighlightActors() { HighlightedActors.Empty(); }

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> HighlightedActors;

	UPROPERTY(BlueprintReadWrite)
	ARPGPlayerController* RPGPlayerController;
	UPROPERTY(BlueprintReadWrite)
	FVector2D StartPosition;
	UPROPERTY(BlueprintReadWrite)
	FVector2D EndPosition;
	UPROPERTY(BlueprintReadWrite)
	bool bIsDrawing;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor MarqueeColor;

};
