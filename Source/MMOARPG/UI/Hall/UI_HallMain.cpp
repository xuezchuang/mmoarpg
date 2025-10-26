// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_HallMain.h"
#include "ThreadManage.h"
//#include "UObject/SimpleController.h"
#include "MMOARPGMacroType.h"
#include "UI_Print.h"
#include "Element/UI_CharacterCreatePanel.h"
#include "Element/UI_RenameCreate.h"
#include "Protocol/HallProtocol.h"
#include "../../Core/Hall/HallPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Element/KneadFace/UI_EditorCharacter.h"
#include "../../Core/Hall/HallPawn.h"
#include "../../Core/Hall/Character/CharacterStage.h"
#include "Element/UI_KneadFace.h"

#define LOCTEXT_NAMESPACE "UUI_HallMain"

void UUI_HallMain::LinkInit()
{
	if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		FMMOARPGUserData& UserData = InGameInstance->GetUserData();
		SEND_DATA(SP_CharacterLogin, UserData.Account, UserData.loingkey);
		UE_LOG(MMOARPG,Display, TEXT("Send SP_CharacterLogin [Account:%s]"), ANSI_TO_TCHAR(UserData.Account.name));
	}
	//	if(InGameInstance_A->GetClient() && InGameInstance_A->GetClient()->GetController())
	//	{
	//		if(FSimpleChannel* SimpleChannel_SIMPLE = InGameInstance_A->GetClient()->GetChannel())
	//		{
	//			TArray<uint8> Buffer;
	//			FSimpleIOStream Stream(Buffer);
	//			FSimpleBunchHead Head;
	//			Head.ProtocolsNumber = (uint16)SP_CharacterLogin;
	//			Stream << Head << UserData.Account << UserData.loingkey;
	//			//Stream.Wirte(UserData.Account.GetCharArray().GetData(), 20);
	//			//Stream.Wirte(UserData.loingkey.GetData(), 11);
				//SimpleChannel_SIMPLE->Send(Buffer);
	//			//FSimpleProtocols<SP_LoginResponses>::Send(SimpleChannel_SIMPLE, __VA_ARGS__);
	//			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("Send SP_CharacterLogin"));
	//			
	//			//FSimpleIOStream FStringStream(UserData.loingkey);
	//			//FString hexString;
	//			//hexString.Empty();
	//			//for(int i = 0; i < 11; i++)
	//			//{
	//			//	hexString += (char)UserData.loingkey[i];
	//			//	if(UserData.loingkey[i] == 0)
	//			//		break;
	//			//}
	//			//hexString.GetCharArray().addu
	//			//FStringStream.Read()
	//			//TCHAR* hexString = reinterpret_cast<TCHAR*>(UserData.loingkey.GetData());
	//			//for(uint8 byte : UserData.loingkey)
	//			//{
	//			//	hexString += FString::Printf(TEXT("%02X"), byte);
	//			//}
	//			//UE_LOG(MMOARPG, Error, TEXT("Send SP_CharacterLogin [Account:%s,loingkey:%s]"), *UserData.Account, *hexString);
	//		}
	//	}
}

UUI_HallMain::UUI_HallMain(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, CAType(ECAType::CA_CREATE)
{

}

void UUI_HallMain::NativeConstruct()
{
	Super::NativeConstruct();

	HallMainIn();

	if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		LinkServer(InGameInstance->GetGateStatus().GateServerAddrInfo);
	}

	UI_RenameCreate->SetParents(this);
	UI_CharacterCreatePanel->SetParents(this);
	UI_EditorCharacter->SetParents(this);

	//PlayRenameOut();
	//ResetCharacterCreatePanel();
}

void UUI_HallMain::NativeDestruct()
{
	Super::NativeDestruct();

}

void UUI_HallMain::PlayEditorCharacterIn()
{
	UI_EditorCharacter->PlayWidgetAnim(TEXT("EditorIn"));
}

void UUI_HallMain::PlayEditorCharacterOut()
{
	UI_EditorCharacter->PlayWidgetAnim(TEXT("EditorOut"));
}

void UUI_HallMain::PlayRenameIn()
{
	UI_RenameCreate->PlayWidgetAnim(TEXT("RenameIn"));
}

void UUI_HallMain::PlayRenameOut()
{
	UI_RenameCreate->PlayWidgetAnim(TEXT("RenameOut"));
}

void UUI_HallMain::ResetCharacterCreatePanel(bool bSpawnNewCharacter)
{
	UI_CharacterCreatePanel->CreateCharacterButtons();

	if (bSpawnNewCharacter)
	{
		//生成最近的对象
		SpawnRecentCharacter();
	}

	HighlightDefaultSelection();
}

void UUI_HallMain::DestroyCharacter()
{
	//删除刚刚角色
	if (AHallPawn* InPawn = GetPawn<AHallPawn>())
	{
		if (InPawn->CharacterStage)
		{
			InPawn->CharacterStage->Destroy();
			InPawn->CharacterStage = nullptr;
		}
	}
}

void UUI_HallMain::SpawnRecentCharacter()
{
	if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
	{
		if (FMMOARPGCharacterAppearance *InCAData = InState->GetRecentCharacter())
		{
			UI_CharacterCreatePanel->SpawnCharacter(InCAData);

			SetEditCharacter(InCAData);
		}
		else
		{
			SetEditCharacter(NULL);
			DestroyCharacter();
		}
	}
}

void UUI_HallMain::HighlightDefaultSelection()
{
	if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
	{
		if (FMMOARPGCharacterAppearance* InCAData = InState->GetRecentCharacter())
		{
			UI_CharacterCreatePanel->HighlightSelection(InCAData->SlotPosition);
		}
	}
}

void UUI_HallMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch(ProtocolNumber)
	{
	case SP_CharacterLogin:
	{
		uint16 childcmd;
		//拿到客户端发送的账号
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> childcmd;
		UE_LOG(MMOARPG, Display, TEXT("Recv SP_CharacterLogin"));
		if(childcmd != 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("Recv SP_CharacterLogin childcmd:%d"));
			UE_LOG(MMOARPG, Error, TEXT("Recv SP_CharacterLogin [childcmd:%d]"), childcmd);
			PrintLog(FString::Printf(TEXT("Recv SP_CharacterLogin [childcmd:%d]"), childcmd));
		}
		else
		{
			if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
			{
				FMMOARPGUserData& UserData = InGameInstance->GetUserData();
				UserData.Reset();
				Stream >> UserData.ID >> UserData.role[0] >> UserData.role[1] >> UserData.role[2];
				//UE_LOG(MMOARPG, Display, TEXT("Recv SP_CharacterLogin [childcmd:%d,mid:%lld]"), childcmd, mid);

				AHallPlayerState* InState = GetPlayerState<AHallPlayerState>();
				FMMOARPGCharacterAppearance tempData; 
				for(int i = 2; i >= 0; i--)
				{
					if(UserData.role[i].isT())
					{
						tempData.Name = UserData.role[i].nick;
						tempData.SlotPosition = i;
						//SetEditCharacter(&tempData);
						InState->AddCharacterCA(tempData);
					}
				}
				UI_CharacterCreatePanel->InitCharacterButtons(InState->GetCharacterAppearance());
				//
				if(tempData.SlotPosition != INDEX_NONE)
				{
					UI_CharacterCreatePanel->SpawnCharacter(&tempData);
					SetEditCharacter(&tempData);
					UI_CharacterCreatePanel->HighlightSelection(tempData.SlotPosition);
				}
				else
				{
					PlayRenameOut();
					ResetCharacterCreatePanel();
				}
			}
		}
		/*
		//if (!CharacterJson.IsEmpty())
		//{
		//	if (AHallPlayerState *InState = GetPlayerState<AHallPlayerState>())
		//	{
		//		NetDataAnalysis::StringToCharacterAppearances(CharacterJson,InState->GetCharacterAppearance());
		//	
		//		UI_CharacterCreatePanel->InitCharacterButtons(InState->GetCharacterAppearance());

		//		//生成最近使用角色
		//		SpawnRecentCharacter();

		//		//让我们高亮
		//		HighlightDefaultSelection();
		//	}
		//}
		*/
		break;
	}
	case SP_CharacterSelect:
	{
		uint16 childcmd;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> childcmd;
		UE_LOG(MMOARPG, Error, TEXT("Recv SP_CharacterSelect [childcmd:%d]"), childcmd);
		break;
	}
	case SP_CreateCharacter:
	{
		uint16 childcmd;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> childcmd;
		if(childcmd == 0)
		{
			if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
			{
				FMMOARPGUserData& UserData = InGameInstance->GetUserData();
				
				uint64  roleid;//角色id
				uint8   roleindex;//角色索引
				uint8   job;//职业
				uint8   sex;//性别
				S_LOGIN_NAME nick;//昵称

				Stream >> roleindex >> roleid >> job >> sex >> nick;
				S_USER_MEMBER_ROLE& mRole = UserData.role[roleindex];
				mRole.id = roleid;
				mRole.job = job;
				mRole.sex = sex;
				FMemory::Memcpy(mRole.nick, &nick, USER_MAX_NICK);

				AHallPlayerState* InState = GetPlayerState<AHallPlayerState>();
				FMMOARPGCharacterAppearance tempData;
				tempData.Name = mRole.nick;
				tempData.SlotPosition = roleindex;
				InState->AddCharacterCA(tempData);
				UI_CharacterCreatePanel->InitCharacterButtons(InState->GetCharacterAppearance());
				if(tempData.SlotPosition != INDEX_NONE)
				{
					UI_CharacterCreatePanel->SpawnCharacter(&tempData);
					SetEditCharacter(&tempData);
					UI_CharacterCreatePanel->HighlightSelection(tempData.SlotPosition);
				}
				else
				{
					PlayRenameOut();
					ResetCharacterCreatePanel();
				}
				
				UE_LOG(MMOARPG, Display, TEXT("Recv SP_CreateCharacter"));
			}
		}
		else
		{
			UE_LOG(MMOARPG, Error, TEXT("Recv SP_CreateCharacter [childcmd:%d]"), childcmd);
		}
		break;
	}
	case SP_DeleteCharacter:
	{
		uint16 childcmd;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> childcmd;
		if(childcmd == 0)
		{
			uint8 roleindex;
			Stream >> roleindex;
			AHallPlayerState* InState = GetPlayerState<AHallPlayerState>();
			InState->RemoveCharacterAppearanceBySlot(roleindex);
			UI_CharacterCreatePanel->InitCharacterButtons(InState->GetCharacterAppearance());
			UE_LOG(MMOARPG, Display, TEXT("Recv SP_DeleteCharacter [roleindex:%d]"), roleindex);
		}
		else
		{
			Stream >> childcmd;
			UE_LOG(MMOARPG, Error, TEXT("Recv SP_DeleteCharacter [childcmd:%d]"), childcmd);
		}
		break;
	}
	case SP_CharacterResponse:
	{
		uint16 childcmd;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> childcmd;
		if(childcmd == 0)
		{
			FMMOARPGUserData& UserData = GetGameInstance<UMMOARPGGameInstance>()->GetUserData();
			int32  userindex;
			Stream >> UserData.base.exp >> UserData.base.econ >> UserData.base.status >> UserData.base.life;
			Stream >> userindex >> UserData.stand.myskill >> UserData.stand.bag;
			UE_LOG(MMOARPG, Display, TEXT("Recv SP_CharacterResponse"));
			HallMainOut();
			//协程
			GThread::Get()->GetCoroutines().BindLambda(
				1.f, [=]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("TestInventory"));
				});
		}
		else
		{
			UE_LOG(MMOARPG, Error, TEXT("Recv SP_CharacterResponse [childcmd:%d]"), childcmd);
		}
		break;
	}
	}

	/*
	//case CharacterSelect:
	//{
	//	ECheckNameType CheckNameType = ECheckNameType::UNKNOWN_ERROR;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_CheckCharacterNameResponses, CheckNameType);
	//	PrintLogByCheckName(CheckNameType);
	//	break;
	//}
	//case SP_CreateCharacterResponses:
	//{
	//	ECheckNameType CheckNameType = ECheckNameType::UNKNOWN_ERROR;
	//	bool bCreateCharacter = false;
	//	FString JsonString;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_CreateCharacterResponses, CheckNameType, bCreateCharacter,JsonString);
	//	if (bCreateCharacter)
	//	{
	//		PrintLog(LOCTEXT("CREATECHARACTERRESPONSES_SUCCESSFULLY", "created successfully."));
	//		
	//		FMMOARPGCharacterAppearance InCA;
	//		NetDataAnalysis::StringToCharacterAppearances(JsonString, InCA);
	//		if (AHallPlayerState* InPlayerState = GetPlayerState<AHallPlayerState>())
	//		{
	//			InPlayerState->AddCharacterCA(InCA);
	//			//淡出
	//			PlayRenameOut();
	//			ResetCharacterCreatePanel(false);
	//			SetEditCharacter(&InCA);
	//		}
	//	}
	//	else
	//	{
	//		PrintLog(LOCTEXT("CREATECHARACTERRESPONSES_FAIL", "created fail."));
	//		GThread::Get()->GetCoroutines().BindLambda(1.5f, [=]() { PrintLogByCheckName(CheckNameType); });
	//	}
	//	
	//	break;
	//}
	//case SP_LoginToDSServerResponses:
	//{
		//FSimpleAddr Addr;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_LoginToDSServerResponses, Addr);

	//	FString DSAddrString = FSimpleNetManage::GetAddrString(Addr);

	//	HallMainOut();
	//	//协程
	//	GThread::Get()->GetCoroutines().BindLambda(1.f, [=]()
	//	{
	//		UGameplayStatics::OpenLevel(GetWorld(),*DSAddrString);
	//	});
	//}
	//case SP_DeleteCharacterResponses:
	//{
	//	int32 UserID = INDEX_NONE;
	//	int32 SlotID = INDEX_NONE;
	//	int32 SuccessDeleteCount = 0;
	//	SIMPLE_PROTOCOLS_RECEIVE(SP_DeleteCharacterResponses, UserID, SlotID, SuccessDeleteCount);

	//	if (SuccessDeleteCount > 2)
	//	{
	//		if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
	//		{
	//			InState->RemoveCharacterAppearanceBySlot(SlotID);

	//			ResetCharacterCreatePanel(true);
	//		}

	//		PrintLog(LOCTEXT("DELETE_CHARACTER_SUCCESS", "The role deletion is successful, and the change operation is irreversible."));
	//	}
	//	else
	//	{
	//		PrintLog(LOCTEXT("DELETE_CHARACTER_ERROR", "Failed to delete the role. Please check if the role exists."));
	//	}
	//}
	//case SP_EditorCharacterResponses:
	//{
	//	FSimpleAddrInfo AddrInfo;
	//	bool bUpdateSucceeded = false;

	//	SIMPLE_PROTOCOLS_RECEIVE(SP_EditorCharacterResponses, bUpdateSucceeded);

	//	if (bUpdateSucceeded)
	//	{
	//		PrintLog(LOCTEXT("EDITORCHARACTERRESPONSES_SUCCESSFULLY", "Edit character succeeded."));
	//	}
	//	else
	//	{
	//		PrintLog(LOCTEXT("EDITORCHARACTERRESPONSES_ERROR", "Edit character Error."));
	//	}

	//	//淡出
	//	PlayRenameOut();
	//	ResetCharacterCreatePanel(false);
	//}
	*/
}

void UUI_HallMain::EditCharacter(int32 InSlot)
{
	PlayRenameIn();
	SetSlotPosition(InSlot);

	if (UUI_KneadFace* InKneadFace = UI_CharacterCreatePanel->CreateKneadFace())
	{
		if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
		{
			if (FMMOARPGCharacterAppearance* InCA = InState->GetCharacterCA(InSlot))
			{
				InKneadFace->InitKneadFace(InCA);
				StartACData = *InCA;

				ResetEidtorType();
				
				//打开动画
				PlayEditorCharacterOut();

				//设置名称
				UI_RenameCreate->SetEditableName(FText::FromString(InCA->Name));
			}
		}
	}
}

void UUI_HallMain::PrintLogByCheckName(ECheckNameType InCheckNameType)
{
	switch (InCheckNameType)
	{
	case UNKNOWN_ERROR:
		PrintLog(LOCTEXT("CHECK_NAME_UNKNOWN_ERROR", "The server encountered an unknown error."));
		break;
	case NAME_NOT_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_NAME_NOT_EXIST", "The name is valid"));
		break;
	case SERVER_NOT_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_SERVER_NOT_EXIST", "Server error."));
		break;
	case NAME_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_NAME_EXIST", "The name has been registered."));
		break;
	}
}

void UUI_HallMain::ResetCharacterAppearance(FMMOARPGCharacterAppearance* InCA)
{
	if (CAType == ECAType::CA_EDITOR)
	{
		if (InCA)
		{
			*InCA = StartACData;
		}
	}

	PlayEditorCharacterIn();
}

//void UUI_HallMain::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
//{
//	if (InType == ESimpleNetErrorType::HAND_SHAKE_SUCCESS)
//	{
//		if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
//		{	
//			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("id:%d"), InGameInstance->GetUserData().ID));
//			//将验证信息发过去
//			FString String;
//			NetDataAnalysis::UserDataToString(InGameInstance->GetUserData(), String);
//
//			//发送角色形象请求
//			//SEND_DATA(SP_CharacterAppearanceRequests, String);
//		}
//	}
//}

void UUI_HallMain::CheckRename(FString& InCharacterName)
{
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		//SEND_DATA(SP_CheckCharacterNameRequests, InGameInstance->GetUserData().ID, InCharacterName);
	}
}

void UUI_HallMain::CreateCharacter(const FMMOARPGCharacterAppearance& InCA)
{
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		FString CAJson;
		if (CAType == ECAType::CA_EDITOR)
		{
			//if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
			//{
			//	if (FMMOARPGCharacterAppearance* InNewCA = InState->GetCharacterCA(UI_RenameCreate->GetSlotPosition()))
			//	{
			//		InNewCA->Date = InCA.Date;
			//		InNewCA->Name = InCA.Name;
			//		NetDataAnalysis::CharacterAppearancesToString(*InNewCA, CAJson);
			//		//SEND_DATA(SP_EditorCharacterRequests, InGameInstance->GetUserData().ID, CAJson);
			//	}
			//}
		}
		else
		{
			NetDataAnalysis::CharacterAppearancesToString(InCA, CAJson);
			//SEND_DATA(SP_CreateCharacterRequests, InGameInstance->GetUserData().ID, CAJson);
			FMMOARPGUserData& UserData = InGameInstance->GetUserData();
			S_ROLE_BASE_INNATE	innate;
			innate.id = UserData.ID;
			innate.job = 0;
			innate.sex = 0;
			FMemory::Memcpy(innate.nick, TCHAR_TO_UTF8(*InCA.Name), USER_MAX_NICK);
			SEND_DATA(SP_CreateCharacter, innate);
		}

		PlayEditorCharacterIn();
	}
}

void UUI_HallMain::ResetEidtorType()
{
	CAType = ECAType::CA_EDITOR;
}

void UUI_HallMain::ResetCreateType()
{
	CAType = ECAType::CA_CREATE;
}

void UUI_HallMain::SetSlotPosition(const int32 InSlotPosition)
{
	UI_RenameCreate->SetSlotPosition(InSlotPosition);
}

void UUI_HallMain::JumpDSServer(int32 InSlotID)
{
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		int64 mid = InGameInstance->GetUserData().ID;
		uint8 slotid = InSlotID;
		SEND_DATA(SP_CharacterSelect, slotid, mid);
	}
}

void UUI_HallMain::LoginCharacter(int32 InSlot)
{

}

void UUI_HallMain::HallMainIn()
{
	PlayWidgetAnim(TEXT("HallMainIn"));
}

void UUI_HallMain::HallMainOut()
{
	PlayWidgetAnim(TEXT("HallMainOut"));
}


void UUI_HallMain::DeleteCharacter(int32 InSlot)
{
	if (InSlot >= 0 && InSlot < 4)
	{
		if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
		{
			FMMOARPGUserData& UserData = InGameInstance->GetUserData();
			uint8	roleindex = InSlot;
			uint64	roleid = UserData.role[roleindex].id;
			SEND_DATA(SP_DeleteCharacter, UserData.ID, roleindex, roleid);

			UE_LOG(MMOARPG, Display, TEXT("Send SP_DeleteCharacter [mid:%lld,rid:%lld,InSlot:%d]"), UserData.ID, roleid, InSlot);
		}
	}
}

void UUI_HallMain::SetEditCharacter(const FMMOARPGCharacterAppearance* InCA)
{
	if (InCA)
	{
		UI_EditorCharacter->SetCharacterName(FText::FromString(InCA->Name));
		UI_EditorCharacter->SetSlotID(InCA->SlotPosition);
	}
	else
	{
		UI_EditorCharacter->SetCharacterName(FText::FromString(""));
		UI_EditorCharacter->SetSlotID(INDEX_NONE);
	}
}

void UUI_HallMain::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void UUI_HallMain::PrintLog(const FText& InMsg)
{
	//播放动画
	UI_Print->PlayTextAnim();

	UI_Print->SetText(InMsg);
}

#undef LOCTEXT_NAMESPACE