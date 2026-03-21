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


//void UUI_Vendor::ShowVendor_Implementation()
//{
//
//}

void UUI_Vendor::VendorBuy(uint16 count, uint32 propid)
{
	SEND_DATA(SP_ItemBuy, count, propid);
	UE_LOG(MMOARPG, Display, TEXT("Send SP_VendorSell"));
}

void UUI_Vendor::VendorSell(uint8 pos, uint16 count, uint32 propid)
{
	SEND_DATA(SP_ItemSell, count, propid);
	UE_LOG(MMOARPG, Display, TEXT("Send SP_VendorSell"));
}

void UUI_Vendor::SetVisibility(ESlateVisibility InVisibility)
{
	if(InVisibility==ESlateVisibility::Visible && InteractionComponent)
	{
		WB_VendorStorageInventory->UpdateInteraction(InteractionComponent);
	}
	//WB_VendorStorageInventory->InteractionComponent = InteractionComponent;
	SetVisibilityInternal(InVisibility);
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
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	// 1. 获取按下的键
	FKey InputKey = InKeyEvent.GetKey();

	// 2. 判断是不是你要处理的键（例如 R 或 V）
	if (InputKey == EKeys::R || InputKey == EKeys::V)
	{
		// 3. 隐藏当前 Widget
		SetVisibility(ESlateVisibility::Hidden);

		// 4. 设置输入模式为 Game + UI
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);

			PC->SetInputMode(InputMode);
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}
