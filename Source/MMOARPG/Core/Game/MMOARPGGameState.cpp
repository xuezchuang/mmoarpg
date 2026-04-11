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

	// Food / Consumable (server prop.csv nick → propid)
	VendorItemIdToPropId.Add(TEXT("Bread"),        200900101);
	VendorItemIdToPropId.Add(TEXT("Beer"),          200900102);
	VendorItemIdToPropId.Add(TEXT("Breadroll"),     200900103);
	VendorItemIdToPropId.Add(TEXT("Carrot"),        200900104);
	VendorItemIdToPropId.Add(TEXT("Cucumber"),      200900105);
	// Fire King armor set (DT_Armor, Description.ID matches server nick)
	VendorItemIdToPropId.Add(TEXT("fs_hm"),        800100107);
	VendorItemIdToPropId.Add(TEXT("fs_p"),         800100109);
	VendorItemIdToPropId.Add(TEXT("fs_g"),         800100106);
	VendorItemIdToPropId.Add(TEXT("fs_pt"),        800100108);
	VendorItemIdToPropId.Add(TEXT("fs_b"),         800100105);
	// Water King armor set
	VendorItemIdToPropId.Add(TEXT("ws_hm"),        800100115);
	VendorItemIdToPropId.Add(TEXT("ws_pt"),        800100117);
	VendorItemIdToPropId.Add(TEXT("ws_gs"),        800100114);
	VendorItemIdToPropId.Add(TEXT("ws_ps"),        800100116);
	VendorItemIdToPropId.Add(TEXT("ws_bt"),        800100113);
	// Elder King accessories
	VendorItemIdToPropId.Add(TEXT("ek_neck_01"),   800100110);
	VendorItemIdToPropId.Add(TEXT("ek_talis_01"),  800100112);
	VendorItemIdToPropId.Add(TEXT("ek_ring_01"),   800100111);
	VendorItemIdToPropId.Add(TEXT("ek_brace_01"),  800100104);
	// DT_Accessory standalone accessories
	VendorItemIdToPropId.Add(TEXT("Necklace01"),   800100102);
	VendorItemIdToPropId.Add(TEXT("Necklace02_Blue"), 800100103);
	VendorItemIdToPropId.Add(TEXT("Necklace02_Red"),  800100101);
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

	ApplyVendorPropIdMap();

	// 构建按 E_InventoryCategory 的分组索引
	BuildCategoryMap();
}

void AMMOARPGGameState::ApplyVendorPropIdMap()
{
	if (VendorItemIdToPropId.Num() == 0)
	{
		return;
	}

	auto NormalizeVendorItemId = [](const FString& InId) -> FString
	{
		FString Out;
		Out.Reserve(InId.Len());
		for (TCHAR Ch : InId)
		{
			if (FChar::IsAlnum(Ch))
			{
				Out.AppendChar(FChar::ToLower(Ch));
			}
		}
		return Out;
	};

	TMap<FString, int32> NormalizedMap;
	NormalizedMap.Reserve(VendorItemIdToPropId.Num());
	for (const auto& Pair : VendorItemIdToPropId)
	{
		NormalizedMap.FindOrAdd(NormalizeVendorItemId(Pair.Key)) = Pair.Value;
	}

	int32 AppliedCount = 0;
	for (auto& Pair : mapDTType2ArrayItem)
	{
		for (FFS_ItemData& Item : Pair.Value)
		{
			if (Item.Index > 0)
			{
				continue;
			}

			const FString ItemId = NormalizeVendorItemId(Item.Description.ID.TrimStartAndEnd());
			if (ItemId.IsEmpty())
			{
				continue;
			}

			if (const int32* FoundPropId = NormalizedMap.Find(ItemId))
			{
				Item.Index = *FoundPropId;
				AppliedCount++;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[VendorMap] Applied propid mapping count=%d mapSize=%d"),
		AppliedCount, VendorItemIdToPropId.Num());
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
