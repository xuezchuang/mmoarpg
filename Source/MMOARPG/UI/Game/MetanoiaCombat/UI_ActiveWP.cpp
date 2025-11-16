// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_ActiveWP.h"

#include "UMG/Public/Components/TextBlock.h"
#include "MMOARPGNetSubsystem.h"
#include "Protocol/HallProtocol.h"
#include "MMOARPGGameInstance.h"
#include "Protocol/GameProtocol.h"

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
	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI)
	{
		return;
	}

	if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
	{
		if (IsSelf)
		{
			InterestingProtos = { SP_CharacterResponse,SP_RoleHP, SP_RoleMP };
			Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_ActiveWP::RecvProtocol, InterestingHandles);
		}
		//else
		//{
		//	//Net->AddProtoListener(InterestingProtos, this, &UUI_ActiveWP::RecvProtocol);
		//}
	}
}

void UUI_ActiveWP::NativeDestruct()
{
	if (auto* Net = GetWorld()->GetGameInstance()->GetSubsystem<UMMOARPGNetSubsystem>())
	{
		Net->RemoveProtoListenersBatch(InterestingProtos, InterestingHandles);
	}
	InterestingProtos.Reset();
	InterestingHandles.Reset();

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

void UUI_ActiveWP::RecvProtocol(uint32 ProtocolNumber)
{
	switch (ProtocolNumber)
	{
		case SP_CharacterResponse:
		{
			UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
			if (GI)
			{
				const FMMOARPGUserData& data = GI->GetUserData();
				RoleInfo.HP = data.base.status.hp;
				RoleInfo.MP = data.base.status.mp;
				RoleInfo.MaxHP = data.base.life.hp;
				RoleInfo.MaxMP = data.base.life.hp;
				RoleInfo.Name = data.base.innate.nick;
				RoleInfo.Level = data.base.exp.level;
				UpdateState();
			}
			break;
		}
	}
}
#undef LOCTEXT_NAMESPACE