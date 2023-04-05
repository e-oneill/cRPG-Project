// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ItemContainerComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemContainerWrapper
{
	GENERATED_BODY();

	FInventoryItemContainerWrapper() {
		RowName = ItemDataHandle.RowName;
	}
	~FInventoryItemContainerWrapper() {
		
		RowName = ItemDataHandle.RowName;
	}

	//is expecting a data table row of FInventoryItemData type
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDataTableRowHandle ItemDataHandle;

	UPROPERTY(VisibleAnywhere, meta = (TitleProperty = "RowName"))
		FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 SetStackSize;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UItemContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemContainerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//issue with RowName not being populated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Item - {SetStackSize}"))
	TArray<FInventoryItemContainerWrapper> Items;

		
};
