// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_SplitStack.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "../MMOARPG.h"

void UUI_SplitStack::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_SplitStack::NativeDestruct()
{
	UnbindHotkeys();
	Super::NativeDestruct();
}

void UUI_SplitStack::InitWithItem(const FFS_ItemData* Item)
{
	if (!Item) return;
	m_ItemData = Item;
	m_Count    = 1;
	m_MaxCount = Item->Stacks.Stackable ? Item->Stacks.Quantity : 1;
	if (m_MaxCount <= 0) m_MaxCount = 99;

	BindHotkeys();
	RefreshUI();
}

void UUI_SplitStack::BindHotkeys()
{
	UnbindHotkeys(); // 防止重复订阅

	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		m_HotkeyHandle = PC->OnVendorHotkey.AddUObject(this, &UUI_SplitStack::OnVendorHotkey);
		// [DBG] 临时日志 — 测试完删除
		UE_LOG(MMOARPG, Display, TEXT("[DBG-SplitStack] BindHotkeys OK Handle=%s PC=%p"),
			m_HotkeyHandle.IsValid() ? TEXT("valid") : TEXT("invalid"), PC);
	}
	else
	{
		// [DBG] 临时日志 — 测试完删除
		UE_LOG(MMOARPG, Warning, TEXT("[DBG-SplitStack] BindHotkeys FAILED: no AMMOARPGPlayerController"));
	}
}

void UUI_SplitStack::UnbindHotkeys()
{
	if (!m_HotkeyHandle.IsValid()) return;

	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		PC->OnVendorHotkey.Remove(m_HotkeyHandle);
	}
	m_HotkeyHandle.Reset();
}

void UUI_SplitStack::OnVendorHotkey(ESystemHotkey Action)
{
	// [DBG] 临时日志 — 测试完删除
	UE_LOG(MMOARPG, Display, TEXT("[DBG-SplitStack] OnVendorHotkey Action=%d Visibility=%d"),
		static_cast<int32>(Action), static_cast<int32>(GetVisibility()));

	// 仅在可见时响应
	if (GetVisibility() == ESlateVisibility::Hidden ||
		GetVisibility() == ESlateVisibility::Collapsed)
	{
		return;
	}

	switch (Action)
	{
	case ESystemHotkey::VendorBuy:           ConfirmBuy();     break;
	case ESystemHotkey::VendorCountDecrease: AddCount(-1);     break;
	case ESystemHotkey::VendorCountIncrease: AddCount(+1);     break;
	case ESystemHotkey::VendorLeave:         SetVisibility(ESlateVisibility::Hidden); break;
	default: break;
	}
}

void UUI_SplitStack::AddCount(int32 Delta)
{
	m_Count = FMath::Clamp(m_Count + Delta, 1, m_MaxCount);
	RefreshUI();
}

void UUI_SplitStack::RefreshUI()
{
	if (!m_ItemData) return;

	if (ItemNameText) ItemNameText->SetText(m_ItemData->Description.Name);
	if (CountText)    CountText->SetText(FText::AsNumber(m_Count));
	if (MaxCountText) MaxCountText->SetText(FText::AsNumber(m_MaxCount));
	if (CostText)     CostText->SetText(FText::AsNumber(FMath::RoundToInt(m_ItemData->Stats.Value * m_Count)));
	if (WeightText)   WeightText->SetText(FText::AsNumber(FMath::RoundToInt(m_ItemData->Stats.Weight * m_Count)));

	// 金币：暂时写死，后续从 PlayerState 读
	if (GoldText)     GoldText->SetText(FText::FromString(TEXT("1,000,000")));
}

void UUI_SplitStack::ConfirmBuy()
{
	if (!m_ItemData) return;

	// 本地测试：直接写入 PlayerState 背包
	// 正式联网时改为：SEND_DATA(SP_ItemBuy, (uint16)m_Count, (uint32)m_ItemData->Index);
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMMOARPGPlayerState* PS = PC->GetPlayerState<AMMOARPGPlayerState>())
		{
			for (int32 i = 0; i < m_Count; ++i)
			{
				FFS_ItemData Copy = *m_ItemData;
				Copy.Stacks.Quantity = 1;
				PS->AddItemToBag(Copy);
			}
		}
	}

	SetVisibility(ESlateVisibility::Hidden);
}
