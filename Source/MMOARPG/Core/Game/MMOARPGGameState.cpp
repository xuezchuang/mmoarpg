// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGGameState.h"
#include "UI/Game/UI_CharacterMenu.h"
#include "Engine/DataTable.h"

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

	// 从编辑器配置的 DataTable 加载所有物品到 mapDTType2ArrayItem
	for (const auto& it : mapDataTable)
	{
		UDataTable* Value = it.Value;
		if (!Value) continue;

		TArray<FFS_ItemData*> aItemP;
		GetTables(Value, aItemP, TEXT("Inventory"));
		if (aItemP.Num() > 0)
		{
			TArray<FFS_ItemData> aItem;
			aItem.Reserve(aItemP.Num());
			for (auto itItem : aItemP)
			{
				aItem.Add(*itItem);
			}
			mapDTType2ArrayItem.Emplace(it.Key, MoveTemp(aItem));
		}
	}

	// 构建按 E_InventoryCategory 的分组索引
	BuildCategoryMap();
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
