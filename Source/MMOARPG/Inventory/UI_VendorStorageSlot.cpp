// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_VendorStorageSlot.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Components/Button.h"
#include "InventoryGameState.h"

//#include <Components/VerticalBox.h>
//#include "UI_CraftingListSlot.h"

void UUI_VendorStorageSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if(m_ItemData)
	{
		AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
		FSlateColor mSlateColor = pGameState->GetRarityColor(m_ItemData->Rarity);
		
		BG->SetColorAndOpacity(mSlateColor.GetSpecifiedColor());
		
		//Quantity->SetText(FText::FromString(FString::Printf(TEXT("%d"),m_ItemData->Stacks.Quantity)));
		Quantity->SetText(FText::FromString(FString::Printf(TEXT("%d"),1)));
		ValueText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"),m_ItemData->Stats.Value)));
		NameText->SetText(m_ItemData->Description.Name);
		//PurchaseMax->SetText(FText::FromString(m_ItemData->Description.Name));
		ItemIcon->SetBrushResourceObject(m_ItemData->Image);
	}
	Native_Button->OnClicked.__Internal_AddDynamic(this, &ThisClass::OnClicked, TEXT("OnClicked"));
}

void UUI_VendorStorageSlot::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_VendorStorageSlot::SetItemData(const FFS_ItemData& pItemData)
{
	m_ItemData = &pItemData;
}

void UUI_VendorStorageSlot::OnClicked()
{
	OnFocus();
	FunUpdateItem.ExecuteIfBound(m_ItemData);
}
