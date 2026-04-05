// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MMOARPGType.h"
#include "../../Data/FItemData.h"
#include "../../Data/FPlayerInventoryData.h"
#include "MMOARPGPlayerState.generated.h"

/**
 *
 */
UCLASS()
class MMOARPG_API AMMOARPGPlayerState : public APlayerState
{
	GENERATED_BODY()


public:
	FMMOARPGCharacterAppearance& GetCA() { return CA; }

	// ===================== 玩家背包 =====================

	/** 获取背包物品列表 */
	TArray<FFS_ItemData>& GetBagItems() { return BagItems; }

	/** 按 UI 分类获取背包物品指针 */
	TArray<FFS_ItemData*> GetBagItemsByCategory(E_InventoryCategory Category);

	/** 添加物品到背包 */
	bool AddItemToBag(const FFS_ItemData& Item);

	/** 从背包移除物品（按索引） */
	bool RemoveItemFromBag(int32 Index);

	/** 背包物品数量 */
	int32 GetBagItemCount() const { return BagItems.Num(); }

private:
	FMMOARPGCharacterAppearance CA;

	/** 玩家背包物品列表（最终由服务器同步，暂时客户端本地管理） */
	UPROPERTY()
	TArray<FFS_ItemData> BagItems;
};
