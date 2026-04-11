// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_VendorStorageSlot.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Components/Button.h"
#include "../Core/Game/MMOARPGGameState.h"
#include "../MMOARPG.h"

//#include <Components/VerticalBox.h>
//#include "UI_CraftingListSlot.h"

void UUI_VendorStorageSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if(m_ItemData)
	{
		AMMOARPGGameState* pGameState = GetGameState<AMMOARPGGameState>();
		if (!pGameState) return;
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
	if (!m_ItemData)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Ignore vendor slot click because item data is null [Widget:%s]"), *GetName());
		return;
	}

	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Vendor slot clicked [Name:%s ID:%s Index:%d Ptr:%p]"),
		*m_ItemData->Description.Name.ToString(),
		*m_ItemData->Description.ID,
		m_ItemData->Index,
		m_ItemData);
	OnFocus();
	FunUpdateItem.ExecuteIfBound(m_ItemData);
}
