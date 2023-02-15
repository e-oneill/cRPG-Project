// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIManagementComponent.generated.h"

class UMainHUD;
class AEncounter;
class UInGameMenu;
class UInventoryComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UUIManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUIManagementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainHUD> HUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInGameMenu> InGameMenuClass;

	UMainHUD* HUD;
	UInGameMenu* InGameMenu;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* FocusWidget;

	AActor* SelectedActor;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddEncounterUI(AEncounter* Encounter);

	void OpenGameMenuUI();

	void CloseGameMenuUI();

	void OpenInventory(UInventoryComponent* InventoryToOpen = nullptr);

	void ChangeSelectedActor(AActor* Actor);
		
public:
	AActor* GetSelectedActor() const { return SelectedActor; }
	void SetSelectedActor(AActor* val) { SelectedActor = val; }
};
