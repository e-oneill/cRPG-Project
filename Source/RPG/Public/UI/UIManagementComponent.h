// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIManagementComponent.generated.h"

class UMainHUD;
class AEncounter;
class UInGameMenu;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIManagerSelectedActorChangeEvent, AActor*, SelectedActor);

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

	UPROPERTY()
	UMainHUD* HUD;
	UPROPERTY()
	UInGameMenu* InGameMenu;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* FocusWidget;

	AActor* SelectedActor;

	bool IsLocallyControlled();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddEncounterUI(AEncounter* Encounter);

	void OpenGameMenuUI();

	void CloseGameMenuUI();

	void OpenInventory(UInventoryComponent* InventoryToOpen = nullptr);

	void ChangeSelectedActor(AActor* Actor);

	AActor* GetSelectedActor() const { return SelectedActor; }
	void SetSelectedActor(AActor* val) { SelectedActor = val; }

	UPROPERTY(BlueprintAssignable)
		FUIManagerSelectedActorChangeEvent OnSelectedChanged;
};
