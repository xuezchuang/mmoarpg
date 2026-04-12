// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Vendor.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPGMacroType.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "UI_WindowSwitcher.h"
#include "UI_VendorStorageInventory.h"
#include "UI_InventoryBase.h"
#include "UI_UpperUIBar.h"
#include "../Core/Component/InteractionComponent.h"
#include "../Core/Game/MMOARPGPlayerController.h"

namespace
{
	void SyncMainHUDVisibility(UWorld* World, ESlateVisibility InVisibility)
	{
		if (!World)
		{
			return;
		}

		if (AMMOARPGPlayerController* PC = World->GetFirstPlayerController<AMMOARPGPlayerController>())
		{
			PC->SetMainUserWidgetVisibility(InVisibility);
		}
	}

	void SetVendorInputMode(UWorld* World, bool bShowCursor)
	{
		if (!World)
		{
			return;
		}

		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (bShowCursor)
			{
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
			else
			{
				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
			}

			PC->bShowMouseCursor = bShowCursor;
		}
	}
}


//void UUI_Vendor::ShowVendor_Implementation()
//{
//
//}

void UUI_Vendor::VendorBuy(uint16 count, uint32 propid)
{
	SEND_DATA(SP_ItemBuy, count, propid);
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Send SP_ItemBuy [count:%u propid:%u]"), count, propid);
}

void UUI_Vendor::VendorSell(uint8 pos, uint16 count, uint32 propid)
{
	SEND_DATA(SP_ItemSell, pos, count, propid);
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Send SP_ItemSell [pos:%u count:%u propid:%u]"), pos, count, propid);
}

void UUI_Vendor::OpenWithInteraction(UInteractionComponent* SourceInteraction)
{
	if (!SourceInteraction)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] OpenWithInteraction failed because SourceInteraction is null"));
		return;
	}

	InteractionComponent = SourceInteraction;
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] OpenWithInteraction [ShopUI:%p SourceInteraction:%p DataTableType:%d]"),
		this,
		InteractionComponent,
		static_cast<int32>(InteractionComponent->DataTableType));

	SetNativeUIType(E_UIType::Buy);

	if (WB_VendorStorageInventory)
	{
		WB_VendorStorageInventory->UpdateInteraction(InteractionComponent);
	}
	else
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] OpenWithInteraction missing WB_VendorStorageInventory [ShopUI:%p]"), this);
	}

	SyncMainHUDVisibility(GetWorld(), ESlateVisibility::Hidden);
	SetVendorInputMode(GetWorld(), true);

	// 商店打开 → 切换到 VendorUI 上下文，屏蔽游戏快捷键
	if (AMMOARPGPlayerController* PC = GetWorld()->GetFirstPlayerController<AMMOARPGPlayerController>())
	{
		PC->SetInputContext(EInputContext::VendorUI);
	}

	SetVisibility(ESlateVisibility::Visible);

	// Blueprint 可能把输入模式覆盖为 UIOnly（NoCapture），导致 InputComponent 不触发。
	// UUI_Vendor::NativeSupportsKeyboardFocus 已返回 true，这里强制抢焦点，
	// 确保 NativeOnKeyDown 能路由 vendor 热键。
	SetKeyboardFocus();
}

void UUI_Vendor::SetVisibility(ESlateVisibility InVisibility)
{
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] SetVisibility [%d] [Interaction:%p DataTableType:%d]"),
		static_cast<int32>(InVisibility),
		InteractionComponent,
		InteractionComponent ? static_cast<int32>(InteractionComponent->DataTableType) : INDEX_NONE);
	SetVisibilityInternal(InVisibility);

	if (InVisibility == ESlateVisibility::Hidden)
	{
		SyncMainHUDVisibility(GetWorld(), ESlateVisibility::Visible);
		SetVendorInputMode(GetWorld(), false);

		// 商店关闭 → 恢复游戏上下文
		if (AMMOARPGPlayerController* PC = GetWorld()->GetFirstPlayerController<AMMOARPGPlayerController>())
		{
			PC->SetInputContext(EInputContext::Game);
		}
	}
}

void UUI_Vendor::NativeConstruct()
{
	Super::NativeConstruct();
	//BindClientRcv();
	//WB_UpperUIBar->SetType(E_UIType::Sell);
	SetNativeUIType(E_UIType::Buy);
	WB_UpperUIBar->BindSwitcherDelegate(this,&UUI_Vendor::SetNativeUIType);
}

void UUI_Vendor::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_Vendor::NativePreConstruct()
{
	Super::NativePreConstruct();
	//WB_UpperUIBar->WindowSwitchDelegate.BindUObject(this, &UUI_Vendor::SetNativeUIType);
	
}

void UUI_Vendor::RecvProtocol(uint32 ProtocolNumber)
{
	//switch(ProtocolNumber)
	//{
	//case SP_VendorBuy:
	//{
	//	uint16 childcmd = 0;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_VendorBuy, childcmd);
	//	UE_LOG(MMOARPG, Display, TEXT("Recv SP_VendorBuy [childcmd:%d]"), childcmd);
	//	break;
	//}
	//case SP_VendorSell:
	//{
	//	uint16 childcmd = 0;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_VendorSell, childcmd);
	//	UE_LOG(MMOARPG, Display, TEXT("Recv SP_VendorSell [childcmd:%d]"), childcmd);
	//	break;
	//}
	//}
}

void UUI_Vendor::SetNativeUIType(E_UIType eType)
{
	if(eType == E_UIType::SellBack)
	{
		WidgetSwitcher->SetActiveWidget(WB_InventoryBase);
		//WB_VendorStorageInventory->SetKeyboardFocus();
	}
	else if(eType == E_UIType::Buy)
	{
		WidgetSwitcher->SetActiveWidget(WB_VendorStorageInventory);
		//WB_InventoryBase->SetKeyboardFocus();
	}
}

FReply UUI_Vendor::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey InputKey = InKeyEvent.GetKey();

	// R / V → 关闭"当前层级"：SplitStack 打开时先关弹窗，否则关整个商店
	if (InputKey == EKeys::R || InputKey == EKeys::V)
	{
		if (WB_VendorStorageInventory && WB_VendorStorageInventory->TryCloseSplitStack())
		{
			// SplitStack 已关，商店保持打开
		}
		else
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		return FReply::Handled();
	}

	// Blueprint 可能将输入模式切换为 UIOnly，导致 PlayerController 的 InputComponent 不触发。
	// 由于 NativeSupportsKeyboardFocus() 返回 true，本 widget 会收到 NativeOnKeyDown，
	// 在这里通过 TryHandleVendorKey 手动路由 vendor 热键。
	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		if (PC->TryHandleVendorKey(InputKey))
		{
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}
