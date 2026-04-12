// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_SplitStack.h"
#include "UI_VendorStorageInventory.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPGMacroType.h"
#include "../Core/Common/MMOARPGNetSubsystem.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "../MMOARPG.h"

namespace
{
	FText FormatSplitStackWeightText(float InValue)
	{
		FNumberFormattingOptions FormatOptions;
		FormatOptions.MinimumFractionalDigits = 0;
		FormatOptions.MaximumFractionalDigits = FMath::IsNearlyEqual(InValue, FMath::RoundToFloat(InValue), KINDA_SMALL_NUMBER) ? 0 : 2;
		return FText::AsNumber(InValue, &FormatOptions);
	}
}

void UUI_SplitStack::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	BindInputWidgets();
	BindNet();
}

void UUI_SplitStack::NativeDestruct()
{
	UnbindNet();
	UnbindInputWidgets();
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
	FocusSplitStack();
}

FReply UUI_SplitStack::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (GetVisibility() == ESlateVisibility::Hidden ||
		GetVisibility() == ESlateVisibility::Collapsed)
	{
		return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	}

	const FKey InputKey = InKeyEvent.GetKey();
	if (InputKey == EKeys::R || InputKey == EKeys::V)
	{
		CloseSplitStack();
		return FReply::Handled();
	}

	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		if (PC->TryHandleVendorKey(InputKey))
		{
			return FReply::Handled();
		}
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UUI_SplitStack::FocusSplitStack()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, this, EMouseLockMode::DoNotLock, true);
		PC->SetShowMouseCursor(true);
	}

	SetKeyboardFocus();
}

void UUI_SplitStack::CloseSplitStack()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (UUI_VendorStorageInventory* VendorPanel = GetParents<UUI_VendorStorageInventory>())
	{
		VendorPanel->FocusVendorPanel();
	}
}

void UUI_SplitStack::BindInputWidgets()
{
	if (DecreaseValueButton)
	{
		DecreaseValueButton->OnClicked.AddDynamic(this, &UUI_SplitStack::OnDecreaseValueButtonClicked);
	}

	if (IncreaseValueButton)
	{
		IncreaseValueButton->OnClicked.AddDynamic(this, &UUI_SplitStack::OnIncreaseValueButtonClicked);
	}

	if (Head_Slider)
	{
		Head_Slider->OnValueChanged.AddDynamic(this, &UUI_SplitStack::OnHeadSliderValueChanged);
	}

	if (EditableTextBox_233)
	{
		EditableTextBox_233->OnTextCommitted.AddDynamic(this, &UUI_SplitStack::OnCountTextCommitted);
	}
}

void UUI_SplitStack::UnbindInputWidgets()
{
	if (DecreaseValueButton)
	{
		DecreaseValueButton->OnClicked.RemoveDynamic(this, &UUI_SplitStack::OnDecreaseValueButtonClicked);
	}

	if (IncreaseValueButton)
	{
		IncreaseValueButton->OnClicked.RemoveDynamic(this, &UUI_SplitStack::OnIncreaseValueButtonClicked);
	}

	if (Head_Slider)
	{
		Head_Slider->OnValueChanged.RemoveDynamic(this, &UUI_SplitStack::OnHeadSliderValueChanged);
	}

	if (EditableTextBox_233)
	{
		EditableTextBox_233->OnTextCommitted.RemoveDynamic(this, &UUI_SplitStack::OnCountTextCommitted);
	}
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

	if (UUI_VendorStorageInventory* VendorPanel = GetParents<UUI_VendorStorageInventory>())
	{
		VendorPanel->NotifyPurchaseSucceeded(m_ItemData, FinalCount);
	}

	if (FSimpleChannel* Channel = GI->GetClient()->GetChannel())
	{
		FSimpleProtocols<SP_InventoryQuery>::Send(Channel);
		UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Request SP_InventoryQuery after buy success [propid:%u count:%d]"),
			BuyPropId, FinalCount);
	}

	CloseSplitStack();
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
	case ESystemHotkey::VendorLeave:         CloseSplitStack(); break;
	default: break;
	}
}

void UUI_SplitStack::AddCount(int32 Delta)
{
	SetCount(m_Count + Delta);
}

void UUI_SplitStack::SetCount(int32 NewCount)
{
	m_Count = FMath::Clamp(NewCount, 1, m_MaxCount);
	RefreshUI();
}

void UUI_SplitStack::OnDecreaseValueButtonClicked()
{
	AddCount(-1);
}

void UUI_SplitStack::OnIncreaseValueButtonClicked()
{
	AddCount(+1);
}

void UUI_SplitStack::OnHeadSliderValueChanged(float InValue)
{
	if (m_bRefreshingUI)
	{
		return;
	}

	const float ClampedValue = FMath::Clamp(InValue, 0.0f, 1.0f);
	const int32 NewCount = (m_MaxCount <= 1)
		? 1
		: FMath::RoundToInt(FMath::Lerp(1.0f, static_cast<float>(m_MaxCount), ClampedValue));
	SetCount(NewCount);
}

void UUI_SplitStack::OnCountTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if (m_bRefreshingUI)
	{
		return;
	}

	if (InCommitMethod != ETextCommit::OnEnter &&
		InCommitMethod != ETextCommit::OnUserMovedFocus &&
		InCommitMethod != ETextCommit::OnCleared)
	{
		return;
	}

	const FString InputString = InText.ToString().TrimStartAndEnd();
	if (InputString.IsEmpty())
	{
		RefreshUI();
		return;
	}

	SetCount(FCString::Atoi(*InputString));
}

void UUI_SplitStack::RefreshUI()
{
	if (!m_ItemData) return;

	m_bRefreshingUI = true;

	if (ItemNameText) ItemNameText->SetText(m_ItemData->Description.Name);
	if (MaxCountText) MaxCountText->SetText(FText::AsNumber(m_MaxCount));
	if (CostText)     CostText->SetText(FText::AsNumber(FMath::RoundToInt(m_ItemData->Stats.Value * m_Count)));
	if (WeightText)   WeightText->SetText(FormatSplitStackWeightText(m_ItemData->Stats.Weight * m_Count));
	if (EditableTextBox_233) EditableTextBox_233->SetText(FText::AsNumber(m_Count));
	if (Head_Slider)
	{
		const float SliderValue = (m_MaxCount <= 1)
			? 0.0f
			: static_cast<float>(m_Count - 1) / static_cast<float>(m_MaxCount - 1);
		Head_Slider->SetValue(SliderValue);
	}

	int32 CurrentGold = 0;
	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		CurrentGold = GI->GetUserData().base.econ.gold;
	}
	if (GoldText) GoldText->SetText(FText::AsNumber(CurrentGold));

	m_bRefreshingUI = false;
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
