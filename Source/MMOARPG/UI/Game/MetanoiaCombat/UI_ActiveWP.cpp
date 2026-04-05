// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_ActiveWP.h"

#include "UMG/Public/Components/TextBlock.h"
#include "MMOARPGNetSubsystem.h"
#include "Protocol/HallProtocol.h"
#include "MMOARPGGameInstance.h"
#include "Protocol/GameProtocol.h"

#define LOCTEXT_NAMESPACE "UI_ActiveWP"

namespace
{
	template <int32 BufferSize>
	FString DecodeFixedUtf8Buffer(const char(&Buffer)[BufferSize])
	{
		ANSICHAR Temp[BufferSize + 1]{};
		FMemory::Memcpy(Temp, Buffer, BufferSize);
		Temp[BufferSize] = '\0';
		return UTF8_TO_TCHAR(Temp);
	}
}

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
		const bool bShouldBindSelfState = !bUseForMount;
		if (bShouldBindSelfState)
		{
			InterestingProtos = { SP_CharacterResponse,SP_RoleHP, SP_RoleMP };
			Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_ActiveWP::RecvProtocol, InterestingHandles);
			RefreshSelfRoleInfo();
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
			RefreshSelfRoleInfo();
			break;
		}
		case SP_RoleHP:
		case SP_RoleMP:
		{
			RefreshSelfRoleInfo();
			break;
		}
	}
}

void UUI_ActiveWP::RefreshSelfRoleInfo()
{
	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI)
	{
		return;
	}

	const FMMOARPGUserData& Data = GI->GetUserData();
	RoleInfo.HP = Data.base.life.hp;
	RoleInfo.MP = Data.base.life.mp;
	RoleInfo.MaxHP = Data.base.life.hp;
	RoleInfo.MaxMP = Data.base.life.mp;
	RoleInfo.Level = Data.base.exp.level;

	const FString AccountName = DecodeFixedUtf8Buffer(Data.Account.name);
	const FString InnateNick = DecodeFixedUtf8Buffer(Data.base.innate.nick);
	int32 CharacterSlot = GI->GetCurrentCharacterSlot();
	if (!(CharacterSlot >= 0 && CharacterSlot < USER_MAX_ROLE && Data.role[CharacterSlot].isT()))
	{
		CharacterSlot = INDEX_NONE;
		for (int32 SlotIndex = 0; SlotIndex < USER_MAX_ROLE; ++SlotIndex)
		{
			if (Data.role[SlotIndex].isT())
			{
				CharacterSlot = SlotIndex;
				break;
			}
		}
	}

	if (CharacterSlot != INDEX_NONE && Data.role[CharacterSlot].nick[0] != '\0')
	{
		RoleInfo.Name = DecodeFixedUtf8Buffer(Data.role[CharacterSlot].nick);
	}
	else if (!InnateNick.IsEmpty())
	{
		RoleInfo.Name = InnateNick;
	}
	else
	{
		RoleInfo.Name = AccountName;
	}

	if (CharacterName)
	{
		CharacterName->SetText(FText::FromString(RoleInfo.Name));
	}

	if (CharacterLevel)
	{
		CharacterLevel->SetText(FText::AsNumber(RoleInfo.Level));
	}

	UpdateState();
}
#undef LOCTEXT_NAMESPACE
