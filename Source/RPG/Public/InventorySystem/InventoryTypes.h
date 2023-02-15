#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryTypes.generated.h"

class UItemEquippable;

UENUM(BlueprintType, Blueprintable)
enum class EEquipmentSlot : uint8
{
	MAINHAND,
	OFFHAND,
	AMMO,
	HEAD,
	NECK,
	TRINKET,
	BODY,
	BELT,
	GLOVES,
	BOOTS
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEquipmentSlot, EEquipmentSlot::MAINHAND, EEquipmentSlot::BOOTS);

USTRUCT(BlueprintType)
struct FEquippedSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EEquipmentSlot Slot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FDataTableRowHandle ItemData;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UItemEquippable* Item;

	/*//optional field. if selected, this will be auto equipped to this slot on begin play
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDataTableRowHandle PreEquippedItem;*/
};