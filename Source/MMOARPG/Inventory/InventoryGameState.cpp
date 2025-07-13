// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryGameState.h"
#include "Engine/DataTable.h"

AInventoryGameState::AInventoryGameState()
{
	//static ConstructorHelpers::FObjectFinder<UDataTable> ArmorTable(TEXT("/Game/DataTable/Inventory/FDT_Armor"));
	//ArmorTablePtr = ArmorTable.Object;

	//static ConstructorHelpers::FObjectFinder<UDataTable> WeaponsTable(TEXT("/Game/DataTable/Inventory/FDT_Weapons"));
	//WeaponTablePtr = WeaponsTable.Object;

	//static ConstructorHelpers::FObjectFinder<UDataTable> _CraftingTablePtr(TEXT("/Game/DataTable/Inventory/FDT_CraftingList"));
	//CraftingTablePtr = _CraftingTablePtr.Object;
}

//TArray<FFS_ItemData*>* AInventoryGameState::GetCookingTables()
//{
//	return GetTables(CookingTablePtr, CookingTables, TEXT("Cooking"));
//}
//
//TArray<FFS_ItemData*>* AInventoryGameState::GetArmorTables()
//{
//	return GetTables(ArmorTablePtr, ArmorTables,TEXT("Weapon"));
//}
//
//TArray<FFS_ItemData*>* AInventoryGameState::GetWeaponTables()
//{
//	return GetTables(WeaponTablePtr, WeaponTables, TEXT("Weapon"));
//}

//FFS_ItemData* AInventoryGameState::GetItemDataByArmorTable(FName RowName)
//{
//	return ArmorTablePtr->FindRow<FFS_ItemData>(RowName, TEXT(""));
//}
//
//FFS_ItemData* AInventoryGameState::GetItemDataByWeaponTable(FName RowName)
//{
//	return WeaponTablePtr->FindRow<FFS_ItemData>(RowName, TEXT(""));
//}

//bool AInventoryGameState::GetCraftingItemListByType(TArray<FFS_ItemData*>& aItemList)
//{
//	aItemList.Reset();
//	if (!mapCrafting.IsEmpty())
//	{
//		mapCrafting.GetKeys(aItemList);
//		return true;
//	}
//	TArray<FFS_ItemData*> aIngredientItemList;
//	GetTables(CraftingTablePtr, CraftingTables, TEXT("Crafting"));
//	FString strNull = TEXT("");
//	FFS_ItemData* ItemData = NULL;
//	for(auto it : CraftingTables)
//	{
//		ItemData = it->CraftableItem.TableAndRow.GetRow<FFS_ItemData>(strNull);
//		if (!ItemData)
//			continue;
//		aItemList.Add(ItemData);
//		aIngredientItemList.Reset();
//		for (auto itReq : it->RequiredItems)
//		{
//			FFS_ItemData* ItemReqData = itReq.TableAndRow.GetRow<FFS_ItemData>(strNull);
//			ItemReqData->Stacks.Quantity = itReq.Quantity;
//			aIngredientItemList.Add(ItemReqData);
//
//		}
//		mapCrafting.Add(ItemData, aIngredientItemList);
//	}
//	return true;
//}
//
//bool AInventoryGameState::GetCraftingReqItemList(FFS_ItemData* ItemData, TArray<FFS_ItemData*>*& aItemList)
//{
//	aItemList = mapCrafting.Find(ItemData);
//	return aItemList != nullptr;
//}
//
//TArray<FFS_ItemData*>* AInventoryGameState::GetCraftingReqItemList()
//{
//	return &CraftingReqItemTables;
//}

TArray<FFS_ItemData>* AInventoryGameState::GetItemsByDataTableType(E_DataTableType DTType)
{
	return mapDTType2ArrayItem.Find(DTType);
}

FSlateColor AInventoryGameState::GetRarityColor(E_ItemRarity ItemRarity)
{
	FSlateColor SlateColor = CommonColor;
	switch (ItemRarity)
	{
	default:
		break;
	case E_ItemRarity::None:
		break;
	case E_ItemRarity::Common:
		SlateColor = CommonColor;
		break;
	case E_ItemRarity::Superior:
		SlateColor = SuperiorColor;
		break;
	case E_ItemRarity::Epic:
		SlateColor = EpicColor;
		break;
	case E_ItemRarity::Legendary:
		SlateColor = LegendaryColor;
		break;
	case E_ItemRarity::Consumable:
		SlateColor = ConsumableColor;
		break;
	case E_ItemRarity::Usable:
		SlateColor = UsableColor;
		break;
	}
	return SlateColor;
}

void AInventoryGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Num:%d"), 666));
	//UDataTable* UCraftingReqItemTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Game/DataTable/Inventory/FDT_CraftingIngredients")));
	//GetTables(UCraftingReqItemTable, CraftingReqItemTables, TEXT("CraftingReq"));
}

const TArray<FFS_ItemData>& AInventoryGameState::GetItemsByInteraction()
{
	//TMap<E_DataTableType, TArray<FFS_ItemData>> mapDTType2ArrayItem;
	return *mapDTType2ArrayItem.Find(E_DataTableType::E_ArmorDT);
}

void AInventoryGameState::BeginPlay()
{
	Super::BeginPlay();
	for(const auto& it : mapDataTable)
	{
		E_DataTableType key = it.Key;
		UDataTable* Value = it.Value;

		TArray<FFS_ItemData*> aItemP;
		if(GetTables(Value, aItemP))
		{
			TArray<FFS_ItemData> aItem;
			aItem.Reserve(aItemP.Num());
			for(auto itItem : aItemP)
			{
				aItem.Add(*itItem);
			}
			//mapDTType2ArrayItem.Add(key, MoveTemp(aItem));
			mapDTType2ArrayItem.Emplace(key, MoveTemp(aItem));
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Num:%d"), 111));

}
