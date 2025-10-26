// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_ActiveWP.h"
//#include "ThreadManage.h"
//#if UE_MMOARPG_DEBUG_DS
//#include "../../Core/Game/Character/MMOARPGPlayerCharacter.h"
//#endif

#include "UMG/Public/Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "UI_ActiveWP"

//void UUI_ActiveWP::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
//{
//	if (InType == ESimpleNetErrorType::HAND_SHAKE_SUCCESS)
//	{
//		
//	}
//}

void UUI_ActiveWP::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateMountUIText();
}

void UUI_ActiveWP::NativeDestruct()
{
	Super::NativeDestruct();


}

void UUI_ActiveWP::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UUI_ActiveWP::UpdateMountUIText(const FText& InMountNameText, const FText& InMountPowerText)
{
	if (bUseForMount)
	{
		MountName->SetVisibility(ESlateVisibility::Visible);
		MountPower->SetVisibility(ESlateVisibility::Visible);
		MountName->SetText(InMountNameText);
		MountPower->SetText(InMountPowerText);
	}
	else
	{
		MountName->SetVisibility(ESlateVisibility::Collapsed);
		MountPower->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_ActiveWP::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
		case 101:
		{
			break;
		}
	}
}
#undef LOCTEXT_NAMESPACE