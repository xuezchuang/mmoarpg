// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingItem.h"
#include "InventoryGameState.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/TextBlock.h"

void UUI_CraftingItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_CraftingItem::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CraftingItem::UpdateItem(const FFS_ItemData* pItemData)
{
	//FSlateBrush SlateBrush = Image_Item->GetBrush();
	//SlateBrush.SetResourceObject(m_ItemData->Image);
	Image_Item->SetBrushResourceObject(pItemData->Image);
	AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
	FSlateColor SlateColor = pGameState->GetRarityColor(pItemData->Rarity);
	Anim_img->SetColorAndOpacity(SlateColor.GetSpecifiedColor());

	if (pItemData->Stacks.Quantity > 1)
	{
		TextQuantity->SetVisibility(ESlateVisibility::Visible);
		TextQuantity->SetText(FText::AsNumber(pItemData->Stacks.Quantity));
	}
	else
	{
		TextQuantity->SetVisibility(ESlateVisibility::Hidden);
	}
}
