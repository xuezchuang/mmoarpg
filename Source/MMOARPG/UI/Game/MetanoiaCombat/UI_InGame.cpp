// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_InGame.h"
#include "Protocol/GameProtocol.h"
#include "Common/MMOARPGNetSubsystem.h"
#include "MMOARPGGameInstance.h"
#include "Protocol/HallProtocol.h"

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

	//UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	//if (GI)
	//{
	//	m_UserData = &GI->GetUserData();
	//}

	//if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
	//{
	//	//InterestingProtos = { SP_CharacterResponse,SP_RoleHP, SP_RoleMP };
 //       //Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_InGame::RecvProtocol, InterestingHandles);
	//}
}

void UUI_InGame::NativeDestruct()
{
    if (auto* Net = GetWorld()->GetGameInstance()->GetSubsystem<UMMOARPGNetSubsystem>())
    {
        Net->RemoveProtoListenersBatch(InterestingProtos, InterestingHandles);
    }
    InterestingProtos.Reset();
    InterestingHandles.Reset();

	Super::NativeDestruct();
}

void UUI_InGame::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UUI_InGame::InitHotkeys(const TMap<FKey, int32>& Hotkeys, const TArray<int32>& Slots)
{

}

void UUI_InGame::RecvProtocol(uint32 ProtocolNumber)
{
	//switch (ProtocolNumber)
	//{
	////case SP_CharacterResponse:
	////{
	////	m_dTolHP = m_UserData->base.life.hp;
	////	m_dTolMP = m_UserData->base.life.mp;
	////	m_dCurHP = m_UserData->base.life.hp;
	////	m_dCurMP = m_UserData->base.life.mp;
	////	UpdateState();
	////	break;
	////}
	//}
}
#undef LOCTEXT_NAMESPACE