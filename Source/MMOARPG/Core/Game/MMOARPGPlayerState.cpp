// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOARPGPlayerState.h"

TArray<FFS_ItemData*> AMMOARPGPlayerState::GetBagItemsByCategory(E_InventoryCategory Category)
{
	TArray<FFS_ItemData*> Result;
	for (FFS_ItemData& Item : BagItems)
	{
		if (GetCategoryFromItemType(Item.Type) == Category)
		{
			Result.Add(&Item);
		}
	}
	return Result;
}

bool AMMOARPGPlayerState::AddItemToBag(const FFS_ItemData& Item)
{
	if (BagItems.Num() >= 150) // USER_MAX_BAG
	{
		return false;
	}
	BagItems.Add(Item);
	return true;
}

bool AMMOARPGPlayerState::RemoveItemFromBag(int32 Index)
{
	if (!BagItems.IsValidIndex(Index))
	{
		return false;
	}
	BagItems.RemoveAt(Index);
	return true;
}
