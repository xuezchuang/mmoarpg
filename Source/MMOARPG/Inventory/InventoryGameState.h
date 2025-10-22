// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DataTable/CharacterAnimTable.h"
#include "DataTable/CharacterStyleTable.h"
#include "FItemData.h"
#include "InventoryGameState.generated.h"



UCLASS()
class AInventoryGameState : public AGameStateBase
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = "Inventory | DataTable")
	//UDataTable* ArmorTablePtr;

	//UPROPERTY(EditDefaultsOnly, Category = "Inventory | DataTable")
	//UDataTable* WeaponTablePtr;

	//UPROPERTY(EditDefaultsOnly, Category = "Inventory | DataTable")
	//UDataTable* CraftingTablePtr;

	//UPROPERTY(EditDefaultsOnly, Category = "Inventory | DataTable")
	//UDataTable* CookingTablePtr;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory | DataTable")
	TMap<E_DataTableType, UDataTable*> mapDataTable;

	//
	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor CommonColor;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor SuperiorColor;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor EpicColor;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor LegendaryColor;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor ConsumableColor;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor UsableColor;

public:
	AInventoryGameState();
	//TArray<FFS_ItemData*>* GetCookingTables();
	//TArray<FFS_ItemData*>* GetArmorTables();
	//TArray<FFS_ItemData*>* GetWeaponTables();
	//TArray<FFS_ItemData*>* GetCraftingReqItemList();
	TArray<FFS_ItemData>* GetItemsByDataTableType(E_DataTableType DTType);
	//FFS_ItemData* GetItemDataByArmorTable(FName RowName);
	//FFS_ItemData* GetItemDataByWeaponTable(FName RowName);
	//bool GetCraftingItemListByType(TArray<FFS_ItemData*>& aItemList);
	//bool GetCraftingReqItemList(FFS_ItemData* ItemData, TArray<FFS_ItemData*>*& aItemList);
	FSlateColor GetRarityColor(E_ItemRarity ItemRarity);

	void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = BP_Inventory)
	const TArray<FFS_ItemData>& GetItemsByInteraction();

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


	virtual void BeginPlay() override;

	//template<class T>
	//T* GetTable(RowName, UDataTable* InTable, TArray<T*>& OutTables, const FString& MsgTag = TEXT("MyTable"))
	//{
	//	if (TArray<T*>* InTables = GetTables<T>(InTable, OutTables, MsgTag))
	//	{
	//		if (InTables->Num())
	//		{
	//			if (auto ItemInTable = InTables->FindByPredicate([&](T* InMyTable) {return InMyTable->ID == InTableID;}))
	//			{
	//				return *ItemInTable;
	//			}
	//		}
	//	}
	//	return NULL;
	//}

protected:
	//TArray<FFS_ItemData*> ArmorTables;
	//TArray<FFS_ItemData*> CookingTables;
	//TArray<FFS_ItemData*> CraftingReqItemTables;
	//TArray<FFS_ItemData*> WeaponTables;
	TArray<FFS_Crafting*> CraftingTables;
	TMap<E_DataTableType, TArray<FFS_ItemData>> mapDTType2ArrayItem;
	TMap<FFS_ItemData*, TArray<FFS_ItemData*>> mapCrafting;
};
