#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryTypes.generated.h"

class UItemEquippable;
class UItemBase;
class UAction;

UENUM(BlueprintType, Blueprintable)
enum class EItemType : uint8
{
	Basic,
	Equippable,
	Consumable
};

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

USTRUCT(BlueprintType)
struct FInventoryLoadSlot
{
	GENERATED_BODY()

public:
	//the item in this slot
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle ItemData;
	//should this item be equipped on BeginPlay
	UPROPERTY(EditDefaultsOnly)
		bool bAutoEquip;
};

USTRUCT(BlueprintType)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY();

	FName RowName;

	#pragma region Base Item
	UPROPERTY(EditAnywhere)
		EItemType ItemType;

	UPROPERTY(EditAnywhere)
		FName ItemName;
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere)
		bool bCanStack;

	UPROPERTY(VisibleInstanceOnly, Category = "Equippable Item", meta = (EditCondition = "bCanStack", EditConditionHides))
		int32 CurrStackSize = 1;

	UPROPERTY(EditAnywhere, Category = "Equippable Item", meta = (EditCondition = "bCanStack", EditConditionHides))
		int32 MaxStackSize;

	#pragma endregion Base Item

	#pragma region Equippable Item
	UPROPERTY(EditAnywhere, Category = "Equippable Item", meta = (EditCondition = "ItemType == EItemType::Equippable", EditConditionHides))
	EEquipmentSlot ValidSlot;
	
	UPROPERTY(EditAnywhere, Category = "Equippable Item", meta = (EditCondition = "ItemType == EItemType::Equippable", EditConditionHides))
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Equippable Item", meta = (EditCondition = "ItemType == EItemType::Equippable", EditConditionHides))
	FName AttachSocket = FName("hand_rSocket");

	UPROPERTY(EditAnywhere, Category = "Equippable Item", meta = (EditCondition = "ItemType == EItemType::Equippable", EditConditionHides))
	TArray<TSoftClassPtr<UAction>> GrantsActions;
	#pragma endregion Equippable Item
	
	#pragma region Consumable Item
	UPROPERTY(EditAnywhere, Category = "Consumable Item", meta = (EditCondition = "ItemType == EItemType::Consumable", EditConditionHides))
	TSoftClassPtr<UAction> ConsumableAction;

	UPROPERTY(EditAnywhere, Category = "Consumable Item", meta = (EditCondition = "ItemType == EItemType::Consumable", EditConditionHides))
	int32 Uses;
	#pragma endregion Consumable Item
	
};