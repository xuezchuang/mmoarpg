// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_LoginMain.h"
#include "MMOARPG.h"
#include "ThreadManage.h"
#include "UObject/SimpleController.h"
#include "UI_Login.h"
#include "Protocol/LoginProtocol.h"
#include "MMOARPGMacroType.h"
#include "UI_Print.h"
#include "Kismet/GameplayStatics.h"
#include "MMOARPGType.h"
#include "UI_Register.h"
#include <Misc/GeneratedTypeName.h>
#include "SocketSubsystem.h"

#define LOCTEXT_NAMESPACE "UUI_LoginMain"

void UUI_LoginMain::NativeConstruct()
{
	Super::NativeConstruct();

	PlayWidgetAnim(TEXT("LoginIn"));

	UI_Login->SetParents(this);
	UI_Register->SetParents(this);

	LinkServer();

	//读取账号
	if (!UI_Login->DecryptionFromLocal(FPaths::ProjectDir() / TEXT("User")))
	{
		PrintLog(TEXT("No account detected."));
	}	
}

void UUI_LoginMain::NativeDestruct()
{
	Super::NativeDestruct();

}

void UUI_LoginMain::SignIn(FString& InAccount,FString& InPassword)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("SP_LoginRequests"));
	if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		S_LOGIN_NAME name;
		S_LOGIN_PASS pass;

		FMemory::Memcpy(&name, TCHAR_TO_UTF8(*InAccount), 20);
		FMemory::Memcpy(&pass, TCHAR_TO_UTF8(*InPassword), 20);

		FMMOARPGUserData& UserData = InGameInstance->GetUserData();
		FMemory::Memcpy(&UserData.Account, TCHAR_TO_UTF8(*InAccount), 20);

		SEND_DATA(SP_LoginResponses, name, pass);
					
	//	if(InGameInstance_A->GetClient() && InGameInstance_A->GetClient()->GetController())
	//	{
	//		if(FSimpleChannel* SimpleChannel_SIMPLE = InGameInstance_A->GetClient()->GetChannel())
	//		{
	//			TArray<uint8> Buffer;
	//			FSimpleIOStream Stream(Buffer);
	//			FSimpleBunchHead Head;
	//			Head.ProtocolsNumber = (uint16)SP_LoginResponses;
	//			Stream << Head;
	//			Stream.Wirte(name.GetData(), 20);
	//			Stream.Wirte(pass.GetData(), 20);
	//			SimpleChannel_SIMPLE->Send(Buffer);
	//			//FSimpleProtocols<SP_LoginResponses>::Send(SimpleChannel_SIMPLE, __VA_ARGS__);
	
	//			//UserData.Account.Empty();
	//			//for(int i = 0;i < 20;i++)
	//			//{
	//			//	UserData.Account += (char)name[i];
	//			//	if(name[i] == 0)
	//			//		break;
	//			//}
	//			//FSimpleIOStream StreamName(name);
	//			//UserData.Account.GetCharArray().AddUninitialized(20);
	//			//StreamName.Read(UserData.Account.GetCharArray().GetData(), 20);
	//			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CMD_100 name:%s"), *UserData.Account));
	//		}
	//	}
	}
}

void UUI_LoginMain::Register()
{
	UI_Register->RegisterIn();
}

void UUI_LoginMain::Register(FString InRegisterInfo)
{
	//SEND_DATA(SP_RegisterRequests, InRegisterInfo);
}

void UUI_LoginMain::Register(FString Name, FString Pass)
{
	S_LOGIN_NAME s_name;
	S_LOGIN_PASS s_pass;

	FMemory::Memcpy(&s_name, TCHAR_TO_UTF8(*Name), 20);
	FMemory::Memcpy(&s_pass, TCHAR_TO_UTF8(*Pass), 20);
	
	SEND_DATA(SP_RegisterResponses, s_name,s_pass);
}

void UUI_LoginMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
		case SP_LoginResponses:
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%s"), TEXT("SP_LoginResponses")));

			if(!UI_Login->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
			{
				PrintLog(TEXT("No account detected."));
			}

			FString String;
			uint16 childcmd = 0;

			//拿到客户端发送的账号
			TArray<uint8> Buffer; 
			Channel->Receive(Buffer);
			FSimpleIOStream Stream(Buffer);
			Stream.Seek(sizeof(FSimpleBunchHead));
			Stream >> childcmd;
			if(childcmd == 0)
			{
				PlayWidgetAnim(TEXT("LoginOut"));
					
				S_LOGIN_KEY loginkey;
				S_LOGIN_IP ip;
				uint16 port;
				Stream >> loginkey >> ip >> port;

				////要关闭我们的 Login服务器
				if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
				{
					if(ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
					{
						//FString str;
						uint32 IP;
						FString DomainName = _T("home.snowsome.com");
						FAddressInfoResult AddressInfoResult = SocketSubsystem->GetAddressInfo(*DomainName, nullptr, EAddressInfoFlags::Default, NAME_None);
						if (AddressInfoResult.Results.Num() > 0)
						{
							TSharedPtr<FInternetAddr> TmpAddr = AddressInfoResult.Results[0].Address;
							//TSharedRef<FInternetAddr> TmpAddr = SocketSubsystem->CreateInternetAddr();
							//bool bBindAddr = false;
							//FString FStr_IP = UTF8_TO_TCHAR(ip.ip);
							//TmpAddr->SetIp(*FStr_IP, bBindAddr);
							TmpAddr->GetIp(IP);

							FMMOARPGGateStatus& gateStatus = InGameInstance->GetGateStatus();
							gateStatus.GateServerAddrInfo.IP = IP;
							gateStatus.GateServerAddrInfo.Port = port;
							FMMOARPGUserData& UserData = InGameInstance->GetUserData();
							UserData.loingkey = loginkey;
							if (InGameInstance->GetClient() && InGameInstance->GetClient()->GetChannel())
							{
								InGameInstance->GetClient()->GetChannel()->DestroySelf();
							}

							//协程
							GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
								{
									UGameplayStatics::OpenLevel(GetWorld(), TEXT("HallMap"));
								});
						}
					}
				}
			}
			else
			{
				UE_LOG(MMOARPG, Error, TEXT("Recv SP_LoginResponses [childcmd:%d]"), childcmd);
				PrintLog(LOCTEXT("LOGIN_WRONG_PASSWORD", "Password verification failed."));
			}
			break;
		}
		case SP_RegisterResponses:
		{
			if(!UI_Login->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
			{
				PrintLog(TEXT("No account detected."));
			}

			ERegistrationType Type = ERegistrationType::SERVER_BUG_WRONG;

			//拿到客户端发送的账号
			SIMPLE_PROTOCOLS_RECEIVE(SP_RegisterResponses, Type);

			switch (Type)
			{
			case ACCOUNT_AND_EMAIL_REPETITION_ERROR:
			{
				PrintLog(LOCTEXT("ACCOUNT_AND_EMAIL_REPETITION_ERROR", "Duplicate account or email."));

				//协程
				GThread::Get()->GetCoroutines().BindLambda(0.8f, [&]()//不支持引用，支持指针 普通结构
				{
					Register();

					//显示重复警告
					UI_Register->ShowDuplicateWarnings();
					UI_Register->ShowFailedtoRegisterWarnings();
				});

				break;
			}
			case PLAYER_REGISTRATION_SUCCESS:
				PrintLog(LOCTEXT("REGISTRATION_SUCCESS", "Registration was successful."));
				break;
			case SERVER_BUG_WRONG:
				PrintLog(LOCTEXT("SERVER_BUG_WRONG", "Server unknown error."));
				break;
			default:
				break;
			}
		}
	}
}

void UUI_LoginMain::LinkInit()
{
	LinkServerInfo(ESimpleNetErrorType::HAND_SHAKE_SUCCESS, FString());
}

void UUI_LoginMain::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if(InType == ESimpleNetErrorType::HAND_SHAKE_SUCCESS)
	{
		UI_LinkWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#undef LOCTEXT_NAMESPACE