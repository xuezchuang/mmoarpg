// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingWindow.h"
#include "InventoryGameState.h"
#include "UMG/Public/Components/Image.h"
#include "UI_CraftingList.h"
#include "UI_CraftingItem.h"
#include "UI_CraftingRequiredItemsBar.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/Widget.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "UMG/Public/Components/ProgressBar.h"
#include "UMG/Public/Components/Button.h"

void UUI_CraftingWindow::NativeConstruct()
{
	Super::NativeConstruct();
	Button_CRAFT->OnClicked.AddDynamic(this, &UUI_CraftingWindow::OnCraft);
	WB_CraftingList->FunUpdateItem.BindUObject(this, &UUI_CraftingWindow::UpdateItem);
	WB_CraftingList->Init();
}

void UUI_CraftingWindow::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CraftingWindow::UpdateItem(const FFS_ItemData* ItemData)
{
	TextDesName->SetText(ItemData->Description.Name);
	FString EnumName = StaticEnum<E_ItemRarity>()->GetNameStringByIndex(static_cast<int32>(ItemData->Rarity));
	TextRatity->SetText(FText::FromString(EnumName));
	TextDescription->SetText(ItemData->Description.Description);
	TextLevel->SetText(FText::AsNumber(ItemData->Stats.RequiredLevel));
	//
	WEIGHT->SetText(FText::FromString("WEIGHT"));
	//Weight_Image->SetBrushResourceObject(ItemData->)
	TextWeight->SetText(FText::AsNumber(ItemData->Stats.Weight));
	//Weight_Quantity_text
	//COST
	// Coins_Image
	//FNumberFormattingOptions NumberFormatOptions;
	//TextValue->SetText(FText::AsNumber(FMath::TruncToInt(ItemData->Stats.Value / 1.5), &NumberFormatOptions));
	TextValue->SetText(FText::AsNumber(FMath::TruncToInt(ItemData->Stats.Value / 1.5)));
	if (ItemData->Stacks.Stackable && ItemData->Stacks.Quantity > 1)
	{
		Box_StackedWeight->SetVisibility(ESlateVisibility::Visible);
		Weight_Quantity_text->SetText(FText::AsNumber(ItemData->Stacks.Quantity * ItemData->Stats.Weight));
	}
	else
	{
		Box_StackedWeight->SetVisibility(ESlateVisibility::Collapsed);
	}
	TextDurability->SetText(FText::AsNumber(ItemData->Stats.Durability.CurrentDurability));
	ProgressBar->SetPercent(ItemData->Stats.Durability.CurrentDurability/100.0);
	// //
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString("111"));
	WB_CraftingItem->UpdateItem(ItemData);
	WB_CraftingRequiredItemsBar->UpdateItem(ItemData);
}

void UUI_CraftingWindow::OnCraft()
{

}

