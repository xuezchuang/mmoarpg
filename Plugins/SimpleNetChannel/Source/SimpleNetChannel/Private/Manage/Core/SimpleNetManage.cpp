#include "SimpleNetManage.h"
#include "../SimpleUDPManage.h"
#include "../SimpleTCPManage.h"
#include "Connetion/Core/SimpleConnetion.h"
#include "IPAddress.h"
#include "Stream/SimpleIOStream.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "Protocols/SimpleNetProtocols.h"
#include "../../Log/SimpleNetChannelLog.h"
#include "SimpleProtocolsDefinition.h"
#include "SocketSubsystem.h"
#include "UObject/SimpleController.h"
#include "Core/EncryptionAndDecryption/md5.h"
#include "Core/EncryptionAndDecryption/SimpleEncryptionAndDecryption.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

#define MAX_MD5_LEN 35
char SafeCode[20] = "testcode";

FSimpleNetManage* FSimpleNetManage::CreateManage(ESimpleNetLinkState InState, ESimpleSocketType InType)
{
	switch(InType)
	{
	case ESimpleSocketType::SIMPLESOCKETTYPE_TCP:
		return new FSimpleTCPManage(InState);
	case ESimpleSocketType::SIMPLESOCKETTYPE_UDP:
		return new FSimpleUDPManage(InState);
	}

	return NULL;
}

bool FSimpleNetManage::Init(int32 InPort)
{
	return false;
}

bool FSimpleNetManage::Init(uint32 InIP, uint32 InPort)
{
	return false;
}

bool FSimpleNetManage::Init(const FString& InIP, uint32 InPort)
{
	return false;
}

bool FSimpleNetManage::Init(const FSimpleAddr& InAddr)
{
	return Init(InAddr.IP, InAddr.Port);
}

void FSimpleNetManage::Tick(float DeltaTime)
{

}

void FSimpleNetManage::Close(const FSimpleAddrInfo& InCloseConnetion)
{
	if(TSharedPtr<FSimpleConnetion> ConnetionInstance = Net[InCloseConnetion.Addr])
	{
		ConnetionInstance->Close();
	}
}

void FSimpleNetManage::VerificationConnectionInfo(TSharedPtr<FSimpleConnetion> InConnetion, uint8* InData, int32 InByteNumber)
{
	if(InConnetion.IsValid())
	{
		FSimpleBunchHead Head = *(FSimpleBunchHead*)InData;
		if(InByteNumber >= sizeof(FSimpleBunchHead))
		{
			if(FSimpleChannel* Channel = InConnetion->GetMainChannel())
			{
				switch(Head.ProtocolsNumber)
				{
				case SP_RCODE:
				{
					char md5[MAX_MD5_LEN];
					InConnetion->rCode = InData[8];
					char a[20];
					sprintf_s(a, "%s_%d", SafeCode, InConnetion->rCode);
					FMemory::Memset(md5, 0, sizeof(md5));
					utils::EncryptMD5str(md5, (unsigned char*)a, int(strlen(a)));

					int32 ID = 0;
					int8 type = 0;
					int32 version = 20180408;
					SIMPLE_PROTOCOLS_SEND(SP_SECURITY, ID, type, version, md5);

					//Send heartbeat

					UE_LOG(LogSimpleNetChannel,
							Display,
							TEXT("[Join] The client receives the welcon message and starts to send heartbeat to the server and [join] request to the server. Server [IP:%s Port:%d]"),
							*InConnetion->GetAddr()->ToString(false),
							InConnetion->GetAddr()->GetPort());
					break;
				}
				case SP_SECURITY:
				{
					uint16 kind = 0;
					int32 Pos = 8;
					FMemory::Memcpy(&InData[Pos], &kind, sizeof(uint16));
					if(kind > 0)
					{
						UE_LOG(LogSimpleNetChannel, Error, TEXT("SP_SECURITY"));
					}
					else
					{
						InConnetion->SetState(ESimpleConnetionLinkType::LINK_ConnectSecure);
					}
					Net.LocalConnetion->StartSendHeartBeat();
					break;
				}
				default:
					break;
				}
			}
		}
		else
		{
			//
		}
	}
}

void FSimpleNetManage::Destroy(FSimpleNetManage* InNetInstance)
{
	if(InNetInstance)
	{
		InNetInstance->Close();

		delete InNetInstance;
		InNetInstance = NULL;
	}
}

USimpleController* FSimpleNetManage::GetController()
{
	if(FSimpleChannel* InChannel = GetChannel())
	{
		return InChannel->GetNetObject<USimpleController>();
	}

	return NULL;
}

USimpleNetworkObject* FSimpleNetManage::GetNetworkObject(uint32 InIP, uint32 InPort, const FGuid& InChannelID)
{
	if(FSimpleChannel* SimpleChannel = GetChannel(InIP, InPort, InChannelID))
	{
		return SimpleChannel->GetNetObject();
	}

	return nullptr;
}

FSimpleChannel* FSimpleNetManage::GetChannel()
{
	return Net.LocalConnetion->GetMainChannel();
}

FSimpleChannel* FSimpleNetManage::GetChannel(uint32 InIP, uint32 InPort, const FGuid& InChannelID)
{
	if(ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem())
	{
		TSharedRef<FInternetAddr> TempLocalAddr = SocketSubsystem->CreateInternetAddr();
		TempLocalAddr->SetIp(InIP);
		TempLocalAddr->SetPort(InPort);
		if(TSharedPtr<FSimpleConnetion> InConnetion = Net[TempLocalAddr])
		{
			return InConnetion->GetChannel(InChannelID);
		}
	}

	return nullptr;
}

bool FSimpleNetManage::CloseSocket()
{
	return false;
}

TStatId FSimpleNetManage::GetStatId() const
{
	return TStatId();
}

void FSimpleNetManage::Close()
{
	Net.LocalConnetion->Close();

	if(LinkState == ESimpleNetLinkState::LINKSTATE_LISTEN)
	{
		for(auto& Tmp : Net.RemoteConnetions)
		{
			Tmp->Close();
		}
	}
}

void FSimpleNetManage::Close(const TSharedPtr<FInternetAddr>& InternetAddr)
{
	if(TSharedPtr<FSimpleConnetion> ConnetionInstance = Net[InternetAddr])
	{
		ConnetionInstance->Close();
	}
}

FSimpleNetManage::FSimpleNetManage()
	:NetworkObjectClass(NULL)
	, SimplePlayerClass(NULL)
	, bInit(false)
{

}

int32 FSimpleNetManage::GetConnetionNum()
{
	int32 Num = 0;
	for(auto& Tmp : Net.RemoteConnetions)
	{
		if(Tmp->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
		{
			Num++;
		}
	}

	return Num;
}

FSimpleAddr FSimpleNetManage::GetAddr()
{
	FSimpleAddr Addr;
	TSharedRef<FInternetAddr>InternetAddr = Net.LocalConnetion->GetAddr();

	//Construct addr
	InternetAddr->GetIp(Addr.IP);
	Addr.Port = InternetAddr->GetPort();

	return Addr;
}

void FSimpleNetManage::LinkTimeout(const FString& InTimeoutMsg)
{
	NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::TIME_OUT, TEXT("Link timeout."));

	Close();
}

void FSimpleNetManage::FNet::Clear(int32 InIndex)
{
	if(InIndex >= 0 && InIndex < RemoteConnetions.Num())
	{
		RemoteConnetions[InIndex]->SetState(ESimpleConnetionLinkType::LINK_Free);
	}
}

int32 FSimpleNetManage::FNet::Add(TSharedPtr<FInternetAddr> InternetAddr)
{
	for(int32 i = 0; i < RemoteConnetions.Num(); i++)
	{
		if(RemoteConnetions[i]->GetState() == ESimpleConnetionLinkType::LINK_Free)
		{
			RemoteConnetions[i]->Init();

			bool bBindAddr = false;
			RemoteConnetions[i]->GetAddr()->SetIp(*InternetAddr->ToString(false), bBindAddr);
			RemoteConnetions[i]->GetAddr()->SetPort(InternetAddr->GetPort());

			RemoteConnetions[i]->SetState(ESimpleConnetionLinkType::LINK_Free);

			return i;
		}
	}

	return INDEX_NONE;
}

bool FSimpleNetManage::FNet::IsAddr(TSharedPtr<FInternetAddr> InternetAddr)
{
	for(auto& Connetion : RemoteConnetions)
	{
		if(*Connetion->GetAddr() == *InternetAddr)
		{
			return true;
		}
	}

	return false;
}

TSharedPtr<FSimpleConnetion> FSimpleNetManage::FNet::operator[](TSharedPtr<FInternetAddr> InternetAddr)
{
	for(auto& Connetion : RemoteConnetions)
	{
		if(*Connetion->GetAddr() == *InternetAddr)
		{
			return Connetion;
		}
	}

	return NULL;
}

TSharedPtr<FSimpleConnetion> FSimpleNetManage::FNet::operator[](const FSimpleAddr& InternetAddr)
{
	for(auto& Tmp : RemoteConnetions)
	{
		uint32 MyPort = Tmp->GetAddr()->GetPort();
		uint32 MyIP = 0;
		Tmp->GetAddr()->GetIp(MyIP);

		if(MyIP == InternetAddr.IP && MyPort == InternetAddr.Port)
		{
			return Tmp;
		}
	}

	return NULL;
}

TSharedPtr<FSimpleConnetion> FSimpleNetManage::FNet::GetEmptyConnetion(TSharedPtr<FInternetAddr> InternetAddr)
{
	for(int32 i = 0; i < RemoteConnetions.Num(); i++)
	{
		if(RemoteConnetions[i]->GetState() == ESimpleConnetionLinkType::LINK_Free)
		{
			if(!RemoteConnetions[i]->IsLock())
			{
				bool bBindAddr;
				RemoteConnetions[i]->GetAddr()->SetIp(*InternetAddr->ToString(false), bBindAddr);
				RemoteConnetions[i]->GetAddr()->SetPort(InternetAddr->GetPort());

				RemoteConnetions[i]->Lock();

				RemoteConnetions[i]->Init();

				return RemoteConnetions[i];
			}
		}
	}

	return NULL;
}

FString FSimpleNetManage::GetAddrString(const FSimpleAddr& InAddr)
{
	FString IPAddrString;
	if(ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem())
	{
		TSharedRef<FInternetAddr> TempLocalAddr = SocketSubsystem->CreateInternetAddr();
		TempLocalAddr->SetIp(InAddr.IP);
		TempLocalAddr->SetPort(InAddr.Port);

		IPAddrString = TempLocalAddr->ToString(true);
	}

	return IPAddrString;
}

FSimpleAddr FSimpleNetManage::GetSimpleAddr(const TCHAR* InIP, uint32 InPort)
{
	FSimpleAddr Addr;
	if(ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem())
	{
		TSharedRef<FInternetAddr> TempLocalAddr = SocketSubsystem->CreateInternetAddr();

		bool bIsValid = false;
		TempLocalAddr->SetIp(InIP, bIsValid);
		if(bIsValid)
		{
			TempLocalAddr->SetPort(InPort);

			TempLocalAddr->GetIp(Addr.IP);
			Addr.Port = TempLocalAddr->GetPort();
		}
	}

	return Addr;
}

void FSimpleNetManage::GetLocalIPAndPort(FString& InIP, uint32& Port)
{
	if(FSimpleNetGlobalInfo::Get()->GetInfo().PublicIP.IsEmpty())
	{
		bool EnableBind = false;
		TSharedRef<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, EnableBind);

		InIP = LocalAddr->ToString(false);
	}
	else
	{
		InIP = FSimpleNetGlobalInfo::Get()->GetInfo().PublicIP;
	}

	Port = 7777;
}

USimpleNetworkObject* FSimpleNetManage::GetNetManageNetworkObject(FSimpleNetManage* InSimpleNetManage, const FSimpleAddrInfo& AddrInfo)
{
	if(InSimpleNetManage)
	{
		if(FSimpleChannel* SimpleChannel = InSimpleNetManage->GetChannel(AddrInfo.Addr.IP, AddrInfo.Addr.Port, AddrInfo.ChannelID))
		{
			return SimpleChannel->GetNetObject();
		}
		else //也有可能是主通道
		{
			FSimpleAddrInfo InMainAddrInfo;
			InSimpleNetManage->GetChannel()->GetAddrInfo(InMainAddrInfo);
			if(InMainAddrInfo == AddrInfo)
			{
				return InSimpleNetManage->GetChannel()->GetNetObject();
			}
		}
	}

	return NULL;
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif