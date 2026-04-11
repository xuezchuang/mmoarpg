// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_SplitStack.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPGMacroType.h"
#include "../Core/Common/MMOARPGNetSubsystem.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "../MMOARPG.h"

void UUI_SplitStack::NativeConstruct()
{
	Super::NativeConstruct();
	BindNet();
}

void UUI_SplitStack::NativeDestruct()
{
	UnbindNet();
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

void UUI_SplitStack::BindNet()
{
	UnbindNet();

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			m_InterestedProtos = { SP_ItemBuy };
			Net->AddProtoListenerBatch(m_InterestedProtos, this, &UUI_SplitStack::RecvProtocol, m_ProtoHandles);
		}
	}
}

void UUI_SplitStack::UnbindNet()
{
	if (m_InterestedProtos.Num() == 0 || m_ProtoHandles.Num() == 0)
	{
		m_InterestedProtos.Reset();
		m_ProtoHandles.Reset();
		return;
	}

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			Net->RemoveProtoListenersBatch(m_InterestedProtos, m_ProtoHandles);
		}
	}

	m_InterestedProtos.Reset();
	m_ProtoHandles.Reset();
}

uint32 UUI_SplitStack::ResolveBuyPropId() const
{
	if (!m_ItemData)
	{
		return 0;
	}

	if (m_ItemData->Index > 0)
	{
		return static_cast<uint32>(m_ItemData->Index);
	}

	const FString RawIdText = m_ItemData->Description.ID;
	const FString IdText = RawIdText.TrimStartAndEnd();
	if (!IdText.IsEmpty() && IdText.IsNumeric())
	{
		return static_cast<uint32>(FCString::Strtoui64(*IdText, nullptr, 10));
	}

	// Normalized item_id (lowercase, alphanumeric only) → server propid.
	UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] ResolveBuyPropId failed [RawID:%s TrimID:%s Index:%d Row:%s Table:%d]"),
		*RawIdText,
		*IdText,
		m_ItemData->Index,
		*m_ItemData->RuntimeRowName.ToString(),
		m_ItemData->RuntimeSourceTableType);
	return 0;
}

void UUI_SplitStack::RecvProtocol(uint32 ProtocolNumber)
{
	if (ProtocolNumber != SP_ItemBuy)
	{
		return;
	}

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI || !GI->GetClient())
	{
		return;
	}

	uint16 ChildCmd = 0;
	int32 BuyCount = 0;
	uint32 BuyPropId = 0;
	SIMPLE_CLIENT_RECV(GI->GetClient(), SP_ItemBuy, ChildCmd, BuyCount, BuyPropId);

	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Recv SP_ItemBuy [childcmd:%u count:%d propid:%u pending:%d]"),
		ChildCmd, BuyCount, BuyPropId, m_bPendingBuy ? 1 : 0);

	if (!m_bPendingBuy)
	{
		return;
	}

	if (ChildCmd != 0)
	{
		const TCHAR* FailReason = TEXT("unknown");
		switch (ChildCmd)
		{
		case 3001: FailReason = TEXT("invalid user"); break;
		case 3002: FailReason = TEXT("user not ready"); break;
		case 3003: FailReason = TEXT("invalid propid"); break;
		case 3004: FailReason = TEXT("invalid count"); break;
		case 3005: FailReason = TEXT("not enough gold"); break;
		case 3006: FailReason = TEXT("create item failed"); break;
		case 3007: FailReason = TEXT("bag full"); break;
		default: break;
		}

		m_bPendingBuy = false;
		m_PendingCount = 0;
		m_PendingPropId = 0;
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Buy failed [childcmd:%u reason:%s propid:%u]"),
			ChildCmd, FailReason, BuyPropId);
		RefreshUI();
		return;
	}

	if (BuyPropId != m_PendingPropId)
	{
		return;
	}

	const int32 FinalCount = BuyCount > 0 ? BuyCount : m_PendingCount;
	if (FinalCount <= 0)
	{
		m_bPendingBuy = false;
		m_PendingCount = 0;
		m_PendingPropId = 0;
		return;
	}

	m_bPendingBuy = false;
	m_PendingCount = 0;
	m_PendingPropId = 0;

	if (FSimpleChannel* Channel = GI->GetClient()->GetChannel())
	{
		FSimpleProtocols<SP_InventoryQuery>::Send(Channel);
		UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Request SP_InventoryQuery after buy success [propid:%u count:%d]"),
			BuyPropId, FinalCount);
	}

	SetVisibility(ESlateVisibility::Hidden);
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

	int32 CurrentGold = 0;
	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		CurrentGold = GI->GetUserData().base.econ.gold;
	}
	if (GoldText) GoldText->SetText(FText::AsNumber(CurrentGold));
}

void UUI_SplitStack::ConfirmBuy()
{
	if (!m_ItemData) return;

	// 本地测试：直接写入 PlayerState 背包
	// 正式联网时改为：SEND_DATA(SP_ItemBuy, (uint16)m_Count, (uint32)m_ItemData->Index);
	int32 BuyCount = m_Count;
	uint32 BuyPropId = ResolveBuyPropId();
	if (BuyPropId == 0)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Block SP_ItemBuy because propid is invalid [Index:%d ID:%s Name:%s]"),
			m_ItemData->Index,
			*m_ItemData->Description.ID,
			*m_ItemData->Description.Name.ToString());
		return;
	}

	m_bPendingBuy = true;
	m_PendingCount = BuyCount;
	m_PendingPropId = BuyPropId;
	m_PendingItemData = *m_ItemData;
	SEND_DATA(SP_ItemBuy, BuyCount, BuyPropId);
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Send SP_ItemBuy [count:%d propid:%u]"), BuyCount, BuyPropId);

}
