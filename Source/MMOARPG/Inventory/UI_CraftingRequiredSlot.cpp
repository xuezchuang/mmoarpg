// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingRequiredSlot.h"

#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/Image.h"
#include "InventoryGameState.h"

void UUI_CraftingRequiredSlot::NativeConstruct()
{
	Super::NativeConstruct();
	Image_Item->SetBrushResourceObject(m_ItemData->Image);
	Text->SetText(m_ItemData->Description.Name);
	Quantity_current->SetText(FText::AsNumber(0));
	Quantity_need->SetText(FText::AsNumber(m_ItemData->Stacks.Quantity));
}
void UUI_CraftingRequiredSlot::NativeDestruct()
{
	Super::NativeDestruct();
}
void UUI_CraftingRequiredSlot::SetItemData(const FFS_ItemData* pItemData)
{
	m_ItemData = pItemData;
}


void UUI_CraftingRequiredSlot::OnClick()
{
}
