// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../../DataTable/CharacterAnimTable.h"
#include "../../DataTable/CharacterStyleTable.h"
#include "../../Data/FCharacterMenuData.h"
#include "../../Data/FItemData.h"
#include "../../Data/FPlayerInventoryData.h"
#include "MMOARPGGameState.generated.h"

class UUI_CharacterMenu;

/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMMOARPGGameState();

	FCharacterAnimTable* GetCharacterAnimTable(int32 InAnimTableID);
	TArray<FCharacterAnimTable*>* GetCharacterAnimTables();

	FCharacterStyleTable* GetCharacterStyleTable(int32 InCharacterTableID);
	TArray<FCharacterStyleTable*>* GetCharacterStyleTables();

	UFUNCTION(BlueprintPure, Category = "UI|CharacterMenu")
	TSubclassOf<UUI_CharacterMenu> GetCharacterMenuWidgetClass() const { return CharacterMenuWidgetClass; }

	UFUNCTION(BlueprintPure, Category = "UI|CharacterMenu")
	FCharacterMenuConfig GetCharacterMenuConfig() const { return CharacterMenuConfig; }
protected:
	template<class T>
	TArray<T*>* GetTables(UDataTable* InTable, TArray<T*>& OutTables, const FString& MsgTag = TEXT("MyTable"))
	{
		if (!OutTables.Num())
		{
			if (InTable)
			{
				InTable->GetAllRows(MsgTag, OutTables);
			}
		}

		return &OutTables;
	}

	template<class T>
	T* GetTable(int32 InTableID, UDataTable* InTable, TArray<T*>& OutTables, const FString& MsgTag = TEXT("MyTable"))
	{
		if (TArray<T*>* InTables = GetTables<T>(InTable, OutTables, MsgTag))
		{
			if (InTables->Num())
			{
				if (auto ItemInTable = InTables->FindByPredicate([&](T* InMyTable) {return InMyTable->ID == InTableID;}))
				{
					return *ItemInTable;
				}
			}
		}

		return NULL;
	}

protected:
	UPROPERTY()
	UDataTable* CharacterAnimTablePtr;

	UPROPERTY()
	UDataTable* CharacterStyleTablePtr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|CharacterMenu")
	TSubclassOf<UUI_CharacterMenu> CharacterMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|CharacterMenu")
	FCharacterMenuConfig CharacterMenuConfig;

	TArray<FCharacterAnimTable*> CharacterAnimTables;
	TArray<FCharacterStyleTable*> CharacterStyleTables;

	// ===================== Inventory（从 InventoryGameState 迁移） =====================
public:
	/** 按 DataTable 类型查询物品列表（商人、制作等用） */
	TArray<FFS_ItemData>* GetItemsByDataTableType(E_DataTableType DTType);

	/** 按 UI 分类查询物品指针列表（玩家背包 UI 分类用） */
	TArray<FFS_ItemData*> GetItemsByCategory(E_InventoryCategory Category);

	/** 按服务器 propid / Index 查找物品模板 */
	const FFS_ItemData* FindItemByIndex(int32 InItemIndex) const;

	/** 获取品质颜色 */
	FSlateColor GetRarityColor(E_ItemRarity ItemRarity);

	virtual void BeginPlay() override;

protected:
	/** 编辑器里配置的 DataTable 映射（按 E_DataTableType 分类） */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|DataTable")
	TMap<E_DataTableType, UDataTable*> mapDataTable;

	/** 商店道具ID（Description.ID）到服务器propid映射，可在蓝图覆写 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Vendor")
	TMap<FString, int32> VendorItemIdToPropId;

	// ---- 品质颜色 ----
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor CommonColor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor SuperiorColor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor EpicColor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor LegendaryColor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor ConsumableColor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	FSlateColor UsableColor;

	/** 按 DataTable 类型存储的物品值数组 */
	TMap<E_DataTableType, TArray<FFS_ItemData>> mapDTType2ArrayItem;

	/** 按 UI 分类存储的物品指针数组（指向 mapDTType2ArrayItem 中的元素） */
	TMap<E_InventoryCategory, TArray<FFS_ItemData*>> mapCategory2Items;

private:
	/** BeginPlay 中加载完 DataTable 后，构建 E_InventoryCategory 分组索引 */
	void BuildCategoryMap();
	void ApplyVendorPropIdMap();
};
