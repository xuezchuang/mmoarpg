// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingListSlot.h"

#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/Image.h"
#include "InventoryGameState.h"

void UUI_CraftingListSlot::NativeConstruct()
{
	Super::NativeConstruct();
	BG_Button->OnClicked.AddDynamic(this, &UUI_CraftingListSlot::OnClick);
	if(m_ItemData)
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("UUI_CraftingListSlot::NativeConstruct Error!"));
	//else
	{
		AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
		FSlateColor mSlateColor = pGameState->GetRarityColor(m_ItemData->Rarity);
		FString ERarityName = StaticEnum<E_ItemRarity>()->GetNameStringByValue(static_cast<int32>(m_ItemData->Rarity));
		FSlateColor BGButtonColor = Common;
		switch (m_ItemData->Rarity)
		{
		default:
			break;
		case E_ItemRarity::None:
			break;
		case E_ItemRarity::Common:
			BGButtonColor = Common;
			break;
		case E_ItemRarity::Superior:
			BGButtonColor = Superior;
			break;
		case E_ItemRarity::Epic:
			BGButtonColor = Epic;
			break;
		case E_ItemRarity::Legendary:
			BGButtonColor = Legendary;
			break;
		case E_ItemRarity::Consumable:
			break;
		case E_ItemRarity::Usable:
			break;
		}
		FButtonStyle BGStyle = BG_Button->GetStyle();
		BGStyle.Hovered.TintColor = BGButtonColor;

		Item_name->SetText(m_ItemData->Description.Name);
		Item_rarity->SetText(FText::FromString(ERarityName));
		Item_rarity->SetColorAndOpacity(mSlateColor.GetSpecifiedColor());
		BG_Button->SetStyle(BGStyle);
		LineColorRarity->SetColorAndOpacity(mSlateColor.GetSpecifiedColor());
		//
		FButtonStyle ImageStyle = Item_Image->GetStyle();
		ImageStyle.Hovered.SetResourceObject(m_ItemData->Image);
		ImageStyle.Normal.SetResourceObject(m_ItemData->Image);
		ImageStyle.Pressed.SetResourceObject(m_ItemData->Image);
		ImageStyle.Hovered.DrawAs = ESlateBrushDrawType::Image;
		ImageStyle.Normal.DrawAs = ESlateBrushDrawType::Image;
		ImageStyle.Pressed.DrawAs = ESlateBrushDrawType::Image;
		Item_Image->SetStyle(ImageStyle);
	}

}

void UUI_CraftingListSlot::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CraftingListSlot::SetItemData(FFS_ItemData* pItemData)
{
	m_ItemData = pItemData;
}

void UUI_CraftingListSlot::SetUpdateItemDelegate(FUpdateItem _FunDelegate)
{
	FunUpdateItem = _FunDelegate;
}

void UUI_CraftingListSlot::OnClick()
{
	if (FunUpdateItem.IsBound())
	{
		FunUpdateItem.Execute(m_ItemData);
	}
}
