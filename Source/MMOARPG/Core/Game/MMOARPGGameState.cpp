// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGGameState.h"
#include "UI/Game/UI_CharacterMenu.h"
#include "Engine/DataTable.h"

namespace
{
	void EnsureInventoryTableLoaded(TMap<E_DataTableType, UDataTable*>& DataTableMap, E_DataTableType TableType, const TCHAR* AssetPath)
	{
		if (DataTableMap.Contains(TableType))
		{
			return;
		}

		if (UDataTable* LoadedTable = LoadObject<UDataTable>(nullptr, AssetPath))
		{
			DataTableMap.Add(TableType, LoadedTable);
		}
	}

	int32 GetGeneratedVendorBucketCode(E_ItemType ItemType)
	{
		switch (ItemType)
		{
		case E_ItemType::Weapon:
		case E_ItemType::Shield:
		case E_ItemType::Bow:
		case E_ItemType::Arrow:
		case E_ItemType::Armor:
		case E_ItemType::Accessories:
		case E_ItemType::Horse_Saddle:
		case E_ItemType::Horse_Armor:
		case E_ItemType::Horse_Reins:
		case E_ItemType::Horse_Wings:
		case E_ItemType::Horse_Horn:
		case E_ItemType::Glider:
			return 10;
		case E_ItemType::Food:
		case E_ItemType::Potion:
		case E_ItemType::Crafting_Ingredient:
		case E_ItemType::Currency:
		case E_ItemType::Other:
		case E_ItemType::Mining_Tool:
		case E_ItemType::Logging_Tool:
		case E_ItemType::Fishing_Tool:
			return 20;
		case E_ItemType::Quest_Item:
		default:
			return 30;
		}
	}

	int32 GetGeneratedVendorTableCode(E_DataTableType DataTableType)
	{
		switch (DataTableType)
		{
		case E_DataTableType::E_WeaponDT: return 1;
		case E_DataTableType::E_ArmorDT: return 2;
		case E_DataTableType::E_Accessory: return 3;
		case E_DataTableType::E_Consumables: return 4;
		case E_DataTableType::E_Cooking: return 5;
		case E_DataTableType::E_Crafting_IngredientDT: return 6;
		case E_DataTableType::E_Ingredients_Fish: return 7;
		case E_DataTableType::E_Gather_Resources: return 8;
		case E_DataTableType::E_Glider: return 9;
		default: return 0;
		}
	}

	int32 BuildGeneratedVendorPropId(E_DataTableType DataTableType, E_ItemType ItemType, int32 Sequence)
	{
		const int32 BucketCode = GetGeneratedVendorBucketCode(ItemType);
		const int32 TableCode = GetGeneratedVendorTableCode(DataTableType);
		if (BucketCode <= 0 || TableCode <= 0 || Sequence <= 0 || Sequence > 9999)
		{
			return 0;
		}

		return 900000000 + (BucketCode * 1000000) + (TableCode * 10000) + Sequence;
	}
}

AMMOARPGGameState::AMMOARPGGameState()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterAnimTable(TEXT("/Game/DataTable/CharacterAnimTable"));
	CharacterAnimTablePtr = CharacterAnimTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterStyleTable(TEXT("/Game/DataTable/CharacterStyleTable"));
	CharacterStyleTablePtr = CharacterStyleTable.Object;

	CharacterMenuConfig.DefaultTab = E_UIType::Character;
	CharacterMenuConfig.Tabs =
	{
		{ E_UIType::Character, NSLOCTEXT("CharacterMenu", "CharacterTab", "Character"), nullptr, 1 }
	};

	// Food / Consumable (server prop.csv nick → propid)
}

FCharacterAnimTable* AMMOARPGGameState::GetCharacterAnimTable(int32 InAnimTableID)
{
	return GetTable(InAnimTableID, CharacterAnimTablePtr, CharacterAnimTables, TEXT("AnimTable"));
}

TArray<FCharacterAnimTable*>* AMMOARPGGameState::GetCharacterAnimTables()
{
	return GetTables(CharacterAnimTablePtr, CharacterAnimTables,TEXT("AnimTable"));
}

FCharacterStyleTable* AMMOARPGGameState::GetCharacterStyleTable(int32 InCharacterTableID)
{
	return GetTable(InCharacterTableID, CharacterStyleTablePtr, CharacterStyleTables, TEXT("CharacterTable"));
}

TArray<FCharacterStyleTable*>* AMMOARPGGameState::GetCharacterStyleTables()
{
	return GetTables(CharacterStyleTablePtr, CharacterStyleTables, TEXT("CharacterTable"));
}

// ===================== Inventory =====================

void AMMOARPGGameState::BeginPlay()
{
	Super::BeginPlay();

	EnsureInventoryTableLoaded(mapDataTable, E_DataTableType::E_Accessory, TEXT("/Game/DataTable/Inventory/FDT_Accessory.FDT_Accessory"));

	// 从编辑器配置的 DataTable 加载所有物品到 mapDTType2ArrayItem
	for (const auto& it : mapDataTable)
	{
		UDataTable* Value = it.Value;
		if (!Value) continue;
		if (Value->GetRowStruct() != FFS_ItemData::StaticStruct()) continue;

		TArray<FName> RowNames = Value->GetRowNames();
		RowNames.Sort([](const FName& A, const FName& B)
		{
			return A.ToString() < B.ToString();
		});

		if (RowNames.Num() > 0)
		{
			TArray<FFS_ItemData> aItem;
			aItem.Reserve(RowNames.Num());
			for (const FName& RowName : RowNames)
			{
				if (const FFS_ItemData* ItemRow = Value->FindRow<FFS_ItemData>(RowName, TEXT("Inventory"), false))
				{
					FFS_ItemData Item = *ItemRow;
					Item.RuntimeRowName = RowName;
					Item.RuntimeSourceTableType = static_cast<int32>(it.Key);
					aItem.Add(MoveTemp(Item));
				}
			}
			mapDTType2ArrayItem.Emplace(it.Key, MoveTemp(aItem));
		}
	}

	ApplyVendorPropIdMap();

	// 构建按 E_InventoryCategory 的分组索引
	BuildCategoryMap();
}

void AMMOARPGGameState::ApplyVendorPropIdMap()
{
	int32 AppliedCount = 0;
	for (auto& Pair : mapDTType2ArrayItem)
	{
		int32 Sequence = 0;
		for (FFS_ItemData& Item : Pair.Value)
		{
			++Sequence;
			Item.RuntimeSourceTableType = static_cast<int32>(Pair.Key);
			Item.Index = BuildGeneratedVendorPropId(Pair.Key, Item.Type, Sequence);
			if (Item.Index > 0)
			{
				AppliedCount++;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[VendorMap] Generated propid mapping count=%d"), AppliedCount);
}

void AMMOARPGGameState::BuildCategoryMap()
{
	mapCategory2Items.Empty();

	for (auto& Pair : mapDTType2ArrayItem)
	{
		for (FFS_ItemData& Item : Pair.Value)
		{
			E_InventoryCategory Cat = GetCategoryFromItemType(Item.Type);
			if (Cat != E_InventoryCategory::None)
			{
				mapCategory2Items.FindOrAdd(Cat).Add(&Item);
			}
		}
	}
}

TArray<FFS_ItemData>* AMMOARPGGameState::GetItemsByDataTableType(E_DataTableType DTType)
{
	return mapDTType2ArrayItem.Find(DTType);
}

TArray<FFS_ItemData*> AMMOARPGGameState::GetItemsByCategory(E_InventoryCategory Category)
{
	if (TArray<FFS_ItemData*>* Found = mapCategory2Items.Find(Category))
	{
		return *Found;
	}
	return TArray<FFS_ItemData*>();
}

const FFS_ItemData* AMMOARPGGameState::FindItemByIndex(int32 InItemIndex) const
{
	if (InItemIndex <= 0)
	{
		return nullptr;
	}

	for (const auto& Pair : mapDTType2ArrayItem)
	{
		const TArray<FFS_ItemData>& Items = Pair.Value;
		if (const FFS_ItemData* Found = Items.FindByPredicate(
			[InItemIndex](const FFS_ItemData& InItem)
			{
				return InItem.Index == InItemIndex;
			}))
		{
			return Found;
		}
	}

	return nullptr;
}

FSlateColor AMMOARPGGameState::GetRarityColor(E_ItemRarity ItemRarity)
{
	switch (ItemRarity)
	{
	case E_ItemRarity::Common:     return CommonColor;
	case E_ItemRarity::Superior:   return SuperiorColor;
	case E_ItemRarity::Epic:       return EpicColor;
	case E_ItemRarity::Legendary:  return LegendaryColor;
	case E_ItemRarity::Consumable: return ConsumableColor;
	case E_ItemRarity::Usable:     return UsableColor;
	default:                       return CommonColor;
	}
}
