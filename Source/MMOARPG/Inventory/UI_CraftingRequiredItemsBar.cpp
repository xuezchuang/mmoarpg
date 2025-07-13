// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_CraftingRequiredItemsBar.h"
#include "MMOARPG.h"
#include "InventoryGameState.h"
#include "UMG/Public/Components/Image.h"
#include "UI_CraftingRequiredSlot.h"
#include "UMG/Public/Components/VerticalBox.h"


UUI_CraftingRequiredItemsBar::UUI_CraftingRequiredItemsBar()
{
	static ConstructorHelpers::FClassFinder<UUI_CraftingRequiredSlot> tBP_CraftingRequiredSlot(TEXT("/Game/MetanoiaCombat/Inventory/UI/Crafting/WB_CraftingRequiredSlot"));
	BP_CraftingRequiredSlot = tBP_CraftingRequiredSlot.Class;
}

void UUI_CraftingRequiredItemsBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_CraftingRequiredItemsBar::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CraftingRequiredItemsBar::UpdateItem(const FFS_ItemData* pItemData)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, pItemData->Description.Name.ToString());
	AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
	//TArray<FFS_ItemData*>* aReqItem = pGameState->GetItemsByDataTableType(E_DataTableType::E_CraftingReqItemDT);
	//
	//VerticalBox->ClearChildren();
	//for(FFS_ItemData* it : *aReqItem)
	//{
	//	UUI_CraftingRequiredSlot* pWidget = CreateWidget<UUI_CraftingRequiredSlot>(GetWorld(), BP_CraftingRequiredSlot);
	//	pWidget->SetItemData(it); 
	//	VerticalBox->AddChildToVerticalBox(pWidget);
	//	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, it->Description.Name.ToString());
	//	//UE_LOG(MMOARPG, Display, TEXT("Hello World"));
	//}
}
