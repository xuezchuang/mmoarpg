// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_GameMain.h"
//#include "ThreadManage.h"
#if UE_MMOARPG_DEBUG_DS
#include "../../Core/Game/Character/MMOARPGPlayerCharacter.h"
#endif

#define LOCTEXT_NAMESPACE "UI_GameMain"

//void UUI_GameMain::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
//{
//	if (InType == ESimpleNetErrorType::HAND_SHAKE_SUCCESS)
//	{
//		
//	}
//}

void UUI_GameMain::NativeConstruct()
{
	Super::NativeConstruct();

#if UE_MMOARPG_DEBUG_DS
	if (AMMOARPGPlayerCharacter *InPlayer = GetPawn<AMMOARPGPlayerCharacter>())
	{
		Robot.GetGateRobot().StartDelegate.BindUObject(InPlayer, &AMMOARPGPlayerCharacter::UpdateKneadingRequest);
	}

	Robot.InitGate("127.0.0.1",11222);
#endif
}

void UUI_GameMain::NativeDestruct()
{
	Super::NativeDestruct();


}

void UUI_GameMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

#if UE_MMOARPG_DEBUG_DS
	Robot.Tick(InDeltaTime);
#endif
}

void UUI_GameMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
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