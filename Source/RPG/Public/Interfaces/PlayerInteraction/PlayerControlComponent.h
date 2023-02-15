// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerControlComponent.generated.h"

class ARPGPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UPlayerControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerControlComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region PlayerConfiguration
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsPlayerControlled;


	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta = (EditCondition = "bIsPlayerControlled"))
		int PlayerIndex;

	UPROPERTY(BlueprintReadWrite)
		ARPGPlayerController* PlayerController;
#pragma endregion PlayerConfiguration


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ARPGPlayerController* GetPlayerController() const { return PlayerController; }
	void SetPlayerController(ARPGPlayerController* val) { PlayerController = val; }
	int GetPlayerIndex() const { return PlayerIndex; }
	void SetPlayerIndex(int val) { PlayerIndex = val; }
	bool IsPlayerControlled() const { return bIsPlayerControlled; }
	void SetIsPlayerControlled(bool val) { bIsPlayerControlled = val; }

};
