#include "UI_InventorySlot.h"
#include "UI_RarityStar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/UserWidget.h"
#include "../Core/Game/MMOARPGGameState.h"

void UUI_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	Item_Button->OnClicked.__Internal_AddDynamic(this, &ThisClass::OnButtonClicked, TEXT("OnButtonClicked"));

	// 默认隐藏 Hover / ActiveBorder
	Hover->SetVisibility(ESlateVisibility::Collapsed);
	ActiveBorder->SetVisibility(ESlateVisibility::Collapsed);

	// 默认关闭商人区域
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
		// 空格：隐藏图标和所有覆盖层，清空星星
		Image_Item->SetBrushResourceObject(nullptr);
		Quantity->SetText(FText::GetEmpty());
		Quantity->SetVisibility(ESlateVisibility::Collapsed);
		RarityColor->SetColorAndOpacity(FLinearColor::Transparent);
		BrokenImage->SetVisibility(ESlateVisibility::Collapsed);
		CloneGridBoost->ClearChildren();
		return;
	}

	// 物品图标
	Image_Item->SetBrushResourceObject(m_ItemData->Image);

	// 数量
	const int32 Qty = m_ItemData->Stacks.Stackable ? m_ItemData->Stacks.Quantity : 1;
	Quantity->SetText(FText::FromString(FString::FromInt(Qty)));
	Quantity->SetVisibility(Qty > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	// 稀有度底色
	AMMOARPGGameState* GS = GetGameState<AMMOARPGGameState>();
	if (GS)
	{
		FSlateColor RaritySlate = GS->GetRarityColor(m_ItemData->Rarity);
		RarityColor->SetColorAndOpacity(RaritySlate.GetSpecifiedColor());
	}

	// 稀有度星星
	RefreshRarityStars(m_ItemData->Rarity);

	// 损坏图标（耐久为 0 时显示）
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

	// 根据稀有度决定显示几颗星
	int32 StarCount = 0;
	switch (Rarity)
	{
	case E_ItemRarity::Common:    StarCount = 1; break;
	case E_ItemRarity::Superior:  StarCount = 2; break;
	case E_ItemRarity::Epic:      StarCount = 3; break;
	case E_ItemRarity::Legendary: StarCount = 4; break;
	default:                       StarCount = 0; break;
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
