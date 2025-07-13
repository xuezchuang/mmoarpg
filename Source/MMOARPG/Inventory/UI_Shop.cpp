// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Shop.h"
#include "Components/Button.h"
#include "InventoryGameState.h"
#include "../Data/FItemData.h"
#include <Components/VerticalBox.h>
#include "UI_CraftingListSlot.h"

void UUI_Shop::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Buy->OnClicked.AddDynamic(this, &UUI_Shop::OnClicked);
	AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
	//TArray<FFS_ItemData*>* aReqItemList = NULL;
	//aReqItemList = pGameState->GetCraftingReqItemList();

	//FUpdateItem BuyItemDelegate;
	//BuyItemDelegate.BindUObject(this, &UUI_Shop::UpdateItem);

	////GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Num:%d"),aReqItemList->Num()));

	//for(FFS_ItemData* ItemData : *aReqItemList)
	//{
	//	UUI_CraftingListSlot* pWidget = CreateWidget<UUI_CraftingListSlot>(GetWorld(), LoadClass<UUI_CraftingListSlot>(this, TEXT("/Game/MetanoiaCombat/Inventory/UI/Crafting/WB_CraftingListSlot.WB_CraftingListSlot_C")));
	//	if(pWidget)
	//	{
	//		pWidget->SetItemData(ItemData);
	//		pWidget->SetUpdateItemDelegate(BuyItemDelegate);
	//		VerticalBox_Shop->AddChildToVerticalBox(pWidget);
	//	}
	//}
}

void UUI_Shop::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_Shop::OnClicked()
{
	

}

void UUI_Shop::UnClicked()
{
}

void UUI_Shop::UpdateItem(FFS_ItemData* ItemData)
{
	m_SelItemData = ItemData;
}
