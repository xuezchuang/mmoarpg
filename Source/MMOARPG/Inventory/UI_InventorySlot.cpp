#include "UI_InventorySlot.h"
#include "UI_RarityStar.h"
#include "UI_ConsumableTypeIcon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/UserWidget.h"
#include "../Core/Game/MMOARPGGameState.h"
#include "../MMOARPG.h"

void UUI_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	Item_Button->OnClicked.__Internal_AddDynamic(this, &ThisClass::OnButtonClicked, TEXT("OnButtonClicked"));

	// Hide hover/active by default.
	Hover->SetVisibility(ESlateVisibility::Collapsed);
	ActiveBorder->SetVisibility(ESlateVisibility::Collapsed);

	// Hide merchant section by default.
	if (MerchantItem)
	{
		MerchantItem->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_InventorySlot::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_InventorySlot::SetItemData(const FFS_ItemData* InItemData)
{
	m_ItemData = InItemData;
	if (!m_ItemData)
	{
		Image_Item->SetVisibility(ESlateVisibility::Collapsed);
		CloneGridBoost->SetVisibility(ESlateVisibility::Collapsed);
		if (WB_Icon_Consumable_Type)
		{
			WB_Icon_Consumable_Type->SetByItemData(nullptr);
		}
		Quantity->SetText(FText::FromString(TEXT("Empty")));
		return;
	}

	// Item icon.
	Image_Item->SetVisibility(ESlateVisibility::HitTestInvisible);
	CloneGridBoost->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (WB_Icon_Consumable_Type)
	{
		WB_Icon_Consumable_Type->SetByItemData(m_ItemData);
	}
	else if ((m_ItemData->Type == E_ItemType::Food) || (m_ItemData->Type == E_ItemType::Potion))
	{
		UE_LOG(MMOARPG, Warning, TEXT("[InventorySlot] WB_Icon_Consumable_Type is null [Item:%s Type:%d Stat:%d]"),
			*m_ItemData->Description.Name.ToString(),
			static_cast<int32>(m_ItemData->Type),
			static_cast<int32>(m_ItemData->Stats.ConsumableAction.Stat));
	}
	Image_Item->SetBrushResourceObject(m_ItemData->Image);

	// Quantity — equipment shows required level, stackables show count.
	const bool bIsEquipment = (m_ItemData->Type == E_ItemType::Armor        ||
	                           m_ItemData->Type == E_ItemType::Weapon        ||
	                           m_ItemData->Type == E_ItemType::Shield        ||
	                           m_ItemData->Type == E_ItemType::Accessories   ||
	                           m_ItemData->Type == E_ItemType::Bow           ||
	                           m_ItemData->Type == E_ItemType::Arrow         ||
	                           m_ItemData->Type == E_ItemType::Horse_Saddle  ||
	                           m_ItemData->Type == E_ItemType::Horse_Armor   ||
	                           m_ItemData->Type == E_ItemType::Horse_Reins   ||
	                           m_ItemData->Type == E_ItemType::Horse_Wings   ||
	                           m_ItemData->Type == E_ItemType::Horse_Horn    ||
	                           m_ItemData->Type == E_ItemType::Glider);
	if (bIsEquipment)
	{
		Quantity->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d"), m_ItemData->Stats.RequiredLevel)));
		Quantity->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		const int32 Qty = m_ItemData->Stacks.Stackable ? m_ItemData->Stacks.Quantity : 1;
		Quantity->SetText(FText::FromString(FString::FromInt(Qty)));
		Quantity->SetVisibility(m_ItemData->Stacks.Stackable ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	// Rarity color.
	AMMOARPGGameState* GS = GetGameState<AMMOARPGGameState>();
	if (GS)
	{
		FSlateColor RaritySlate = GS->GetRarityColor(m_ItemData->Rarity);
		RarityColor->SetColorAndOpacity(RaritySlate.GetSpecifiedColor());
	}

	// Rarity stars.
	RefreshRarityStars(m_ItemData->Rarity);

	// Broken icon when durability reaches zero.
	const bool bBroken = m_ItemData->Stats.Durability.UseDurability
	                     && (m_ItemData->Stats.Durability.CurrentDurability <= 0);
	BrokenImage->SetVisibility(bBroken ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UUI_InventorySlot::SetMerchantMode(bool bMerchant, int32 InSellQuantity)
{
	if (MerchantItem)
	{
		MerchantItem->SetVisibility(bMerchant ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (SellQuantity && bMerchant)
	{
		SellQuantity->SetText(FText::FromString(FString::Printf(TEXT("x%d"), InSellQuantity)));
	}
}

void UUI_InventorySlot::SetActive(bool bActive)
{
	ActiveBorder->SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UUI_InventorySlot::OnButtonClicked()
{
	OnSlotClicked.ExecuteIfBound(m_ItemData);
}

void UUI_InventorySlot::RefreshRarityStars(E_ItemRarity Rarity)
{
	if (!CloneGridBoost || !RarityStarClass)
	{
		return;
	}

	CloneGridBoost->ClearChildren();

	// Decide star count based on rarity.
	int32 StarCount = 0;
	switch (Rarity)
	{
	case E_ItemRarity::Common:    StarCount = 1; break;
	case E_ItemRarity::Superior:  StarCount = 2; break;
	case E_ItemRarity::Epic:      StarCount = 3; break;
	case E_ItemRarity::Legendary: StarCount = 4; break;
	default:                      StarCount = 0; break;
	}

	for (int32 i = 0; i < StarCount; ++i)
	{
		UUI_RarityStar* Star = CreateWidget<UUI_RarityStar>(this, RarityStarClass);
		if (Star)
		{
			UUniformGridSlot* GridSlot = CloneGridBoost->AddChildToUniformGrid(Star, 0, i);
			if (GridSlot)
			{
				GridSlot->SetHorizontalAlignment(HAlign_Center);
				GridSlot->SetVerticalAlignment(VAlign_Center);
			}
		}
	}
}
