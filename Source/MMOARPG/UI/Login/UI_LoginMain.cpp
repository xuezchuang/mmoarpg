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
#include "MMOARPGNetSubsystem.h"

#define LOCTEXT_NAMESPACE "UUI_LoginMain"

void UUI_LoginMain::NativeConstruct()
{
	Super::NativeConstruct();

	PlayWidgetAnim(TEXT("LoginIn"));

	UI_Login->SetParents(this);
	UI_Register->SetParents(this);

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();

	//LinkServer();
	if (GI)
    {
        if (auto* NetSub = GI->GetSubsystem<UMMOARPGNetSubsystem>())
        {
			NetSub->RegisterUniqueHandler(SP_LoginResponses, FProtocolHandler::CreateUObject(this, &UUI_LoginMain::RecvProtocol));
			NetSub->RegisterUniqueHandler(SP_RegisterResponses, FProtocolHandler::CreateUObject(this, &UUI_LoginMain::RecvProtocol));

			NetSub->OnNetLinked.BindUObject(this, &UUI_LoginMain::LinkInit);

			NetSub->BeginLink(ENetServerRole::Login);

        }
    }

	// 快速测试模式：自动登录
	if (GI && GI->bEnableQuickTest)
	{
		UE_LOG(LogTemp, Display, TEXT("[QuickTest] Auto login with account: %s"), *GI->QuickTestAccount);

		// 自动填充账号密码
		UI_Login->SetAccountText(FText::FromString(GI->QuickTestAccount));
		UI_Login->SetPasswordText(FText::FromString(GI->QuickTestPassword));

		// 延迟一点调用登录，确保网络已连接
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindWeakLambda(this, [this]()
		{
			// 使用UI_Login的公共方法获取文本
			FString AccountStr = UI_Login->GetAccountText().ToString();
			FString PasswordStr = UI_Login->GetPasswordText().ToString();
			SignIn(AccountStr, PasswordStr);
		});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0f, false);
		return;
	}

	//读取账号
	if (!UI_Login->DecryptionFromLocal(FPaths::ProjectDir() / TEXT("User")))
	{
		PrintLog(TEXT("No account detected."));
	}
}

void UUI_LoginMain::NativeDestruct()
{
	Super::NativeDestruct();

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (auto* NetSub = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			NetSub->UnRegisterUniqueHandler(SP_LoginResponses);
			NetSub->UnRegisterUniqueHandler(SP_RegisterResponses);

			NetSub->OnNetLinked.Unbind();
		}
	}

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
		UE_LOG(MMOARPG, Display, TEXT("[LoginUI] Send SP_LoginResponses [Account:%s]"), *InAccount);

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
						//FString DomainName = _T("home.snowsome.com");
						//FAddressInfoResult AddressInfoResult = SocketSubsystem->GetAddressInfo(*DomainName, nullptr, EAddressInfoFlags::Default, NAME_None);
						//if (AddressInfoResult.Results.Num() > 0)
						{
							//TSharedPtr<FInternetAddr> TmpAddr = AddressInfoResult.Results[0].Address;
							TSharedRef<FInternetAddr> TmpAddr = SocketSubsystem->CreateInternetAddr();
							bool bBindAddr = false;
							FString FStr_IP = UTF8_TO_TCHAR(ip.ip);
							TmpAddr->SetIp(*FStr_IP, bBindAddr);
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

void UUI_LoginMain::LinkInit(ENetServerRole ServerRole)
{
	ensure(ServerRole == ENetServerRole::Login);
	UI_LinkWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_LoginMain::HandleRegisterResponse(ERegistrationType Type)
{
	switch (Type)
	{
	case ACCOUNT_AND_EMAIL_REPETITION_ERROR:
	{
		//PrintLog(LOCTEXT("ACCOUNT_AND_EMAIL_REPETITION_ERROR", "Duplicate account or email."));

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

void UUI_LoginMain::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void UUI_LoginMain::PrintLog(const FText& InMsg)
{
	//播放动画
	UI_Print->PlayTextAnim();

	UI_Print->SetText(InMsg);
}

#undef LOCTEXT_NAMESPACE
