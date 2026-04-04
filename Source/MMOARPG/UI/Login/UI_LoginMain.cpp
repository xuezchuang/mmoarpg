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

	// Quick test mode: auto-fill credentials and trigger login.
	if (GI && GI->bEnableQuickTest)
	{
		UE_LOG(LogTemp, Display, TEXT("[QuickTest] Auto login with account: %s"), *GI->QuickTestAccount);

		UI_Login->SetAccountText(FText::FromString(GI->QuickTestAccount));
		UI_Login->SetPasswordText(FText::FromString(GI->QuickTestPassword));

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindWeakLambda(this, [this]()
		{
			FString AccountStr = UI_Login->GetAccountText().ToString();
			FString PasswordStr = UI_Login->GetPasswordText().ToString();
			SignIn(AccountStr, PasswordStr);
		});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0f, false);
		return;
	}

	// Read cached account info from local storage.
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
			if(!UI_Login->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
			{
				PrintLog(TEXT("No account detected."));
			}

			uint16 childcmd = 0;

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

				if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
				{
					if(ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
					{
						uint32 IP;
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

						GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
						{
							UGameplayStatics::OpenLevel(GetWorld(), TEXT("HallMap"));
						});
					}
				}
			}
			else
			{
				UE_LOG(MMOARPG, Error, TEXT("Recv SP_LoginResponses [childcmd:%d]"), childcmd);
				switch (childcmd)
				{
				case 1001:
					PrintLog(LOCTEXT("LOGIN_WRONG_PASSWORD", "Password verification failed."));
					break;
				case 1002:
					PrintLog(LOCTEXT("LOGIN_ACCOUNT_NOT_REGISTERED", "The account is not registered."));
					break;
				case 1003:
					PrintLog(LOCTEXT("LOGIN_GATE_NOT_READY", "No gate server is currently available. Please try again later."));
					break;
				default:
					PrintLog(FText::Format(
						LOCTEXT("LOGIN_UNKNOWN_ERROR_WITH_CODE", "Login failed. Error code: {0}"),
						FText::AsNumber(childcmd)));
					break;
				}
			}
			break;
		}
		case SP_RegisterResponses:
		{
			if(!UI_Login->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
			{
				PrintLog(TEXT("No account detected."));
			}

			uint16 ErrorCode = 0;
			SIMPLE_PROTOCOLS_RECEIVE(SP_RegisterResponses, ErrorCode);
			UE_LOG(MMOARPG, Display, TEXT("Recv SP_RegisterResponses [errcode:%d]"), ErrorCode);
			HandleRegisterResponse(ErrorCode);
			break;
		}
	}
}

void UUI_LoginMain::LinkInit(ENetServerRole ServerRole)
{
	ensure(ServerRole == ENetServerRole::Login);
	UI_LinkWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_LoginMain::HandleRegisterResponse(uint16 ErrorCode)
{
	switch (ErrorCode)
	{
	case 0:
		PrintLog(LOCTEXT("REGISTRATION_SUCCESS", "Registration was successful."));
		break;
	case 1001:
		PrintLog(LOCTEXT("ACCOUNT_AND_EMAIL_REPETITION_ERROR", "Duplicate account or email."));
		GThread::Get()->GetCoroutines().BindLambda(0.8f, [&]()
		{
			Register();
			UI_Register->ShowDuplicateWarnings();
			UI_Register->ShowFailedtoRegisterWarnings();
		});
		break;
	case 1099:
		PrintLog(LOCTEXT("REGISTRATION_DB_WRITE_FAILED", "Registration failed while writing to the server database."));
		break;
	default:
		PrintLog(FText::Format(
			LOCTEXT("REGISTRATION_UNKNOWN_ERROR_WITH_CODE", "Registration failed. Error code: {0}"),
			FText::AsNumber(ErrorCode)));
		break;
	}
}

void UUI_LoginMain::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void UUI_LoginMain::PrintLog(const FText& InMsg)
{
	// Play the text animation before updating the message.
	UI_Print->PlayTextAnim();

	UI_Print->SetText(InMsg);
}

#undef LOCTEXT_NAMESPACE
