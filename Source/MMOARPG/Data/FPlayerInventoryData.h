#pragma once
#include "CoreMinimal.h"
#include "Data/FItemData.h"
#include "FPlayerInventoryData.generated.h"

/**
 * 玩家背包 UI 分类枚举
 * 对应 UI_InventoryBase 中的 10 个分类按钮和 UniformGrid_P1~P10
 */
UENUM(BlueprintType)
enum class E_InventoryCategory : uint8
{
	None        = 0  UMETA(DisplayName = "None"),
	Weapon      = 1  UMETA(DisplayName = "Weapon"),       // P1: Weapon, Shield
	Range       = 2  UMETA(DisplayName = "Range"),        // P2: Bow, Arrow
	Armor       = 3  UMETA(DisplayName = "Armor"),        // P3: Armor
	Accessories = 4  UMETA(DisplayName = "Accessories"),  // P4: Accessories
	RuneStone   = 5  UMETA(DisplayName = "RuneStone"),    // P5: Rune items
	Consumable  = 6  UMETA(DisplayName = "Consumable"),   // P6: Food, Potion
	Resources   = 7  UMETA(DisplayName = "Resources"),    // P7: Crafting, Currency, Tools
	Mount       = 8  UMETA(DisplayName = "Mount"),        // P8: Horse_*, Glider
	Quest       = 9  UMETA(DisplayName = "Quest"),        // P9: Quest_Item
	Event       = 10 UMETA(DisplayName = "Event"),        // P10: Event items
};

/**
 * 根据 E_ItemType 推导所属 UI 分类
 */
static E_InventoryCategory GetCategoryFromItemType(E_ItemType ItemType)
{
	switch (ItemType)
	{
	case E_ItemType::Weapon:
	case E_ItemType::Shield:
		return E_InventoryCategory::Weapon;

	case E_ItemType::Bow:
	case E_ItemType::Arrow:
		return E_InventoryCategory::Range;

	case E_ItemType::Armor:
		return E_InventoryCategory::Armor;

	case E_ItemType::Accessories:
		return E_InventoryCategory::Accessories;

	case E_ItemType::Food:
	case E_ItemType::Potion:
		return E_InventoryCategory::Consumable;

	case E_ItemType::Crafting_Ingredient:
	case E_ItemType::Currency:
	case E_ItemType::Other:
	case E_ItemType::Mining_Tool:
	case E_ItemType::Logging_Tool:
	case E_ItemType::Fishing_Tool:
		return E_InventoryCategory::Resources;

	case E_ItemType::Horse_Saddle:
	case E_ItemType::Horse_Armor:
	case E_ItemType::Horse_Reins:
	case E_ItemType::Horse_Wings:
	case E_ItemType::Horse_Horn:
	case E_ItemType::Glider:
		return E_InventoryCategory::Mount;

	case E_ItemType::Quest_Item:
		return E_InventoryCategory::Quest;

	default:
		return E_InventoryCategory::None;
	}
}
