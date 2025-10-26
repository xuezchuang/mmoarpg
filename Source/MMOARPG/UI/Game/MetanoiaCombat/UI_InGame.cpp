// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_InGame.h"
//#include "ThreadManage.h"

#define LOCTEXT_NAMESPACE "UI_InGame"

//void UUI_InGame::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
//{
//	if (InType == ESimpleNetErrorType::HAND_SHAKE_SUCCESS)
//	{
//		
//	}
//}

void UUI_InGame::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUI_InGame::NativeDestruct()
{
	Super::NativeDestruct();


}

void UUI_InGame::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UUI_InGame::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
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