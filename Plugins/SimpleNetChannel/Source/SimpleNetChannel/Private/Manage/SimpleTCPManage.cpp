#include "SimpleTCPManage.h"
#include "../Connetion/SimpleTCPConnetion.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "../Log/SimpleNetChannelLog.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "Thread/SimpleNetThread.h"
#include "Protocols/SimpleNetProtocols.h"
#include "Core/EncryptionAndDecryption/SimpleEncryptionAndDecryption.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif


FSimpleTCPManage::FSimpleTCPManage(ESimpleNetLinkState InType)
	:Super()
{
	Socket = NULL;

	bEndThread = false;

	LinkState = InType;
}

bool FSimpleTCPManage::Init(int32 InPort)
{
	return Init(0, InPort);
}

bool FSimpleTCPManage::CloseSocket()
{
	ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem();
	if(!SocketSubsystem)
	{
		FString ErrorInfo = TEXT("Failed to create socketSubsystem.");
		NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::INIT_FAIL, ErrorInfo);
		UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);

		return false;
	}

	if(Socket)
	{
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;

		return true;
	}

	return true;
}

bool FSimpleTCPManage::Init(const FString& InIP, uint32 InPort)
{
	ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem();
	if(SocketSubsystem)
	{
		auto NewAddrTmp = SocketSubsystem->CreateInternetAddr();

		bool bBindAddr = false;
		NewAddrTmp->SetIp(*InIP, bBindAddr);
		uint32 UintIP;
		NewAddrTmp->GetIp(UintIP);

		return Init(UintIP, InPort);
	}

	return false;
}

bool FSimpleTCPManage::Init(uint32 InIP, uint32 InPort)
{
	Super::Init(InIP, InPort);

	ResetLocalConnetion();

	if(Net.LocalConnetion)
	{
		ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem();
		if(!SocketSubsystem)
		{
			FString ErrorInfo = TEXT("Failed to create socketSubsystem.");
			NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::INIT_FAIL, ErrorInfo);
			UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);

			return false;
		}

		if(!CloseSocket())
		{
			UE_LOG(LogSimpleNetChannel, Error, TEXT("Failed to close the socket. It may have been closed. This message can be ignored."));
		}

		Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("RenZhai"));
		if(!Socket)
		{
			FString ErrorInfo = TEXT("Failed to create Socket.");
			NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::INIT_FAIL, ErrorInfo);
			UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);

			return false;
		}
		UE_LOG(LogSimpleNetChannel, Display, TEXT("Create socket:%d"), Socket->GetPortNo());
		int32 RecvSize = 0;
		int32 SendSize = 0;
		Socket->SetReceiveBufferSize(FSimpleNetGlobalInfo::Get()->GetInfo().RecvDataNumber, RecvSize);
		Socket->SetSendBufferSize(FSimpleNetGlobalInfo::Get()->GetInfo().SendDataNumber, SendSize);

		//Is it monitoring
		switch(LinkState)
		{
		case ESimpleNetLinkState::LINKSTATE_LISTEN://Server
		{

			Net.LocalConnetion->GetAddr()->SetAnyAddress();
			Net.LocalConnetion->GetAddr()->SetPort(InPort == 0 ? FSimpleNetGlobalInfo::Get()->GetInfo().Port : InPort);

			int32 BoundPort = SocketSubsystem->BindNextPort(Socket, *Net.LocalConnetion->GetAddr(), 1, 1);
			if(!BoundPort)
			{
				FString ErrorInfo = TEXT("Server failed to bind port.");
				NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::INIT_FAIL, ErrorInfo);

				UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);
				SocketSubsystem->DestroySocket(Socket);
				return false;
			}

			if(!Socket->Listen(10))
			{
				FString ErrorInfo = TEXT("Server failed to bind port.");
				UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);
				return false;
			}

			break;
		}
		case ESimpleNetLinkState::LINKSTATE_CONNET://Client
		{
			bool bBindAddr = false;
			if(InIP == 0)
			{
				FString DomainName = *FSimpleNetGlobalInfo::Get()->GetInfo().URL;
				FAddressInfoResult AddressInfoResult = SocketSubsystem->GetAddressInfo(*DomainName, nullptr, EAddressInfoFlags::Default, NAME_None);
				if (AddressInfoResult.Results.Num() > 0)
				{
					TSharedPtr<FInternetAddr> AddrPtr = AddressInfoResult.Results[0].Address;
					//= SocketSubsystem->GetHostByName(DomainName)->GetResolvedAddress();
					if (AddrPtr.IsValid())  // 确保解析成功
					{
						AddrPtr->GetIp(InIP);
						//Net.LocalConnetion->GetAddr()->SetIp(AddrPtr.ToSharedRef());
					}
					//Net.LocalConnetion->GetAddr() = SocketSubsystem->GetAddressFromString(DomainName).ToSharedRef();
					//Net.LocalConnetion->GetAddr()->SetIp(*FSimpleNetGlobalInfo::Get()->GetInfo().URL, bBindAddr);
				}
			}
			Net.LocalConnetion->GetAddr()->SetIp(InIP);

			Net.LocalConnetion->GetAddr()->SetPort(InPort == 0 ? FSimpleNetGlobalInfo::Get()->GetInfo().Port : InPort);
			break;
		}
		}

		if(!FSimpleNetGlobalInfo::Get()->GetInfo().bAsynchronous)
		{
			if(!Socket->SetNonBlocking())
			{
				FString ErrorInfo = TEXT("Setting non blocking mode failed.");
				NetManageMsgDelegate.ExecuteIfBound(ESimpleNetErrorType::INIT_FAIL, ErrorInfo);

				UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *ErrorInfo);
				SocketSubsystem->DestroySocket(Socket);
				return false;
			}
		}

		//Register socket
		Net.LocalConnetion->SetSocket(Socket);

		Net.LocalConnetion->SetLinkState(LinkState);

		Net.LocalConnetion->Init();

		//Client ready to connect
		Net.LocalConnetion->ConnectVerification();

		bInit = true;
	}

	return true;
}

void FSimpleTCPManage::Tick(float DeltaTime)//Both the client and the server listen for messages
{
	Super::Tick(DeltaTime);

	if(bInit) //Initialization completed
	{
		if(Socket)
		{
			if(Net.LocalConnetion.IsValid())
			{
				//tick
				if(Net.LocalConnetion->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
				{
					Net.LocalConnetion->Tick(DeltaTime);
				}
				else //Check whether the client times out
				{
					if(LinkState == ESimpleNetLinkState::LINKSTATE_CONNET)
					{
						Net.LocalConnetion->CheckLoginTimeout(DeltaTime);
					}
				}

				for(auto& Tmp : Net.RemoteConnetions)
				{
					if(Tmp->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
					{
						Tmp->Tick(DeltaTime);
					}
				}

				//Do you want to turn on asynchronous
				if(!FSimpleNetGlobalInfo::Get()->GetInfo().bAsynchronous)
				{
					Listen();
				}
			}
		}
	}
}

void FSimpleTCPManage::Listen()
{
	if(!Socket)
	{
		return;
	}

	int32 RecvDataNumber = FSimpleNetGlobalInfo::Get()->GetInfo().RecvDataNumber;
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	uint8 Data[8196 * 1024] = { 0 };
#elif /*PLATFORM_IOS || */PLATFORM_ANDROID
	uint8 Data[8196 * 10] = { 0 };
#else
	uint8 Data[8196 * 100] = { 0 };
#endif
	int32 BytesRead = 0;
	bool bRecvFrom = Socket->Recv(Data, RecvDataNumber, BytesRead);
	if(bRecvFrom && BytesRead > 0)
	{
		SimpleEncryptionAndDecryption::Decryption(Data, BytesRead, Net.LocalConnetion->rCode);

		FGuid InGUID;

		if(Net.LocalConnetion->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
		{
			Net.LocalConnetion->Analysis(Data, BytesRead);
		}
		else
		{
			VerificationConnectionInfo(Net.LocalConnetion, Data, BytesRead);
		}
		if(InGUID != FGuid())
		{
			Caches.Remove(InGUID);
		}
	}
}

FSimpleTCPManage::FCache::FCache()
{
	Reset();
}

bool FSimpleTCPManage::IsCompletePackage(int32 InRecvNum,
										 uint8* InData,
										 TSharedPtr<FSimpleConnetion> InConnetion,
										 FGuid& OutGUID,
										 uint8*& OutData,
										 int32& OutLen)
{
	if(InConnetion.IsValid())
	{
		if(InRecvNum != 0)
		{
			bool bShowCompletePackProtocolInfo = FSimpleNetGlobalInfo::Get()->GetInfo().bShowCompletePackProtocolInfo;
			bool bSlidingWindow = FSimpleNetGlobalInfo::Get()->GetInfo().bSlidingWindow;
			if(bSlidingWindow)
			{
				FSimplePackageHead PackageHead = *(FSimplePackageHead*)InData;
				if(PackageHead.bForceSend)
				{
					if(FSimpleTCPManage::FCache* InCache = Caches.Find(PackageHead.PackageID))
					{
						Caches.Remove(PackageHead.PackageID);

						if(bShowCompletePackProtocolInfo)
						{
							UE_LOG(LogSimpleNetChannel, Display, TEXT("ForceRecv PackageID=%s"),
								   *PackageHead.PackageID.ToString());
						}
					}
					else
					{
						//Remove the head
						Caches.Add(PackageHead.PackageID, FCache());

						if(FSimpleTCPManage::FCache* InUPDCache = Caches.Find(PackageHead.PackageID))
						{
							int32 BoySize = InRecvNum - sizeof(FSimplePackageHead);
							InData += sizeof(FSimplePackageHead);
							int32 Pos = InUPDCache->Cache.AddUninitialized(BoySize);
							FMemory::Memcpy(&InUPDCache->Cache[Pos], InData, BoySize);
							OutData = InUPDCache->Cache.GetData();
							OutLen = InUPDCache->Cache.Num();
							OutGUID = PackageHead.PackageID;

							if(bShowCompletePackProtocolInfo)
							{
								UE_LOG(LogSimpleNetChannel, Display, TEXT("ForceRecv PackageID=%s RecvSize = %i BoySize = %i"),
									   *PackageHead.PackageID.ToString(),
									   InRecvNum,
									   BoySize);
							}
						}
						else
						{
							UE_LOG(LogSimpleNetChannel, Error, TEXT("ForceRecv not found PackageID = %s "), *PackageHead.PackageID.ToString());
						}
					}
				}
				else
				{
					switch(PackageHead.Protocol)
					{
					case SP_HandshaketoSend:
					{
						PackageHead.Protocol = SP_ReadytoAccept;
						TArray<uint8> MyData;

						FSimpleIOStream IOStream(MyData);
						IOStream << PackageHead;

						//Create accepted cache pool
						Caches.Add(PackageHead.PackageID, FSimpleTCPManage::FCache());
						Caches[PackageHead.PackageID].TotalSize = PackageHead.PackageSize;

						InConnetion->Send(MyData);

						if(bShowCompletePackProtocolInfo)
						{
							UE_LOG(LogSimpleNetChannel, Display, TEXT("[SP_HandshaketoSend] PackageHead.PackageID=%s, ReadyRecv=%i"),
								   *PackageHead.PackageID.ToString(),
								   Caches[PackageHead.PackageID].TotalSize);
						}
						break;
					}
					case SP_ReadytoAccept:
					{
						if(FSimpleChannel* InChannel = InConnetion->GetChannel(PackageHead.ChannelID))
						{
							InChannel->SendBatch(PackageHead.PackageID, PackageHead.PackageIndex, SP_Recv);

							if(bShowCompletePackProtocolInfo)
							{
								UE_LOG(LogSimpleNetChannel, Display, TEXT("[SP_ReadytoAccept] PackageHead.ChannelID=%s,PackageHead.PackageIndex=%i"),
									   *PackageHead.ChannelID.ToString(),
									   PackageHead.PackageIndex);
							}
						}
						else
						{
							UE_LOG(LogSimpleNetChannel, Error, TEXT("[SP_ReadytoAccept] Not found PackageHead.ChannelID=%s"),
								   *PackageHead.ChannelID.ToString());
						}

						break;
					}
					case SP_RecvComplete:
					{
						if(FSimpleChannel* InChannel = InConnetion->GetChannel(PackageHead.ChannelID))
						{
							InChannel->AcceptSuccess(PackageHead.PackageID, PackageHead.PackageIndex - 1, true, true);

							if(bShowCompletePackProtocolInfo)
							{
								UE_LOG(LogSimpleNetChannel, Display, TEXT("[SP_RecvComplete] PackageHead.ChannelID=%s,PackageHead.PackageIndex=%i"),
									   *PackageHead.ChannelID.ToString(),
									   PackageHead.PackageIndex);
							}
						}
						else
						{
							UE_LOG(LogSimpleNetChannel, Error, TEXT("[SP_RecvComplete] Not found PackageHead.ChannelID=%s"),
								   *PackageHead.ChannelID.ToString());
						}

						break;
					}
					case SP_Send:
					{
						if(FSimpleChannel* InChannel = InConnetion->GetChannel(PackageHead.ChannelID))
						{
							//Last package accepted successfully
							InChannel->AcceptSuccess(PackageHead.PackageID, PackageHead.PackageIndex - 1, true);

							//Send the next packet
							InChannel->SendBatch(PackageHead.PackageID, PackageHead.PackageIndex, SP_Recv);

							if(bShowCompletePackProtocolInfo)
							{
								UE_LOG(LogSimpleNetChannel, Display, TEXT("[SP_Send] PackageHead.ChannelID=%s,PackageHead.PackageIndex=%i"),
									   *PackageHead.ChannelID.ToString(),
									   PackageHead.PackageIndex);
							}
						}
						else
						{
							UE_LOG(LogSimpleNetChannel, Error, TEXT("[SP_Send] Not found PackageHead.ChannelID=%s"),
								   *PackageHead.ChannelID.ToString());
						}

						break;
					}
					case SP_Recv:
					{
						if(FSimpleTCPManage::FCache* InCache = Caches.Find(PackageHead.PackageID))
						{
							int32 InDataSize = InRecvNum - sizeof(FSimplePackageHead);
							int32 Pos = InCache->Cache.AddUninitialized(InDataSize);

							InData += sizeof(FSimplePackageHead);

							FMemory::Memcpy(&InCache->Cache[Pos], InData, InDataSize);

							TArray<uint8> MyData;
							if(InCache->Cache.Num() < (int32)InCache->TotalSize)//Representative data not accepted
							{
								//Continue sending
								PackageHead.Protocol = SP_Send;
								PackageHead.PackageIndex++;
							}
							else
							{
								PackageHead.Protocol = SP_RecvComplete;
								OutData = InCache->Cache.GetData();
								OutLen = InCache->Cache.Num();
								OutGUID = PackageHead.PackageID;
							}

							FSimpleIOStream IOStream(MyData);
							IOStream << PackageHead;

							InConnetion->Send(MyData);

							if(bShowCompletePackProtocolInfo)
							{
								UE_LOG(LogSimpleNetChannel, Display, TEXT("[SP_Recv] PackageHead.ChannelID=%s,PackageHead.PackageIndex=%i Protocol=%s"),
									   *PackageHead.PackageID.ToString(),
									   PackageHead.PackageIndex,
									   PackageHead.Protocol == SP_Send ? TEXT("SP_Send") :
									   PackageHead.Protocol == SP_RecvComplete ? TEXT("SP_RecvComplete") : TEXT("ERROR"));
							}
						}

						break;
					}
					}
				}
			}
			else
			{
				OutData = InData;
				OutLen = InRecvNum;
			}
		}
	}

	return OutData != nullptr && OutLen != 0;
}

//iocp
void FSimpleTCPManage::Close()
{
	Super::Close();

	if(Socket)
	{
		if(ISocketSubsystem* SocketSubsystem = FSimpleConnetion::GetSocketSubsystem())
		{
			SocketSubsystem->DestroySocket(Socket);
			Socket = nullptr;
		}
	}

	bEndThread = true;
}

void FSimpleTCPManage::Close(const FSimpleAddrInfo& InCloseConnetion)
{
	Super::Close(InCloseConnetion);



}

void FSimpleTCPManage::ResetLocalConnetion()
{
	Net.LocalConnetion = MakeShareable(new FSimpleTCPConnetion());
	Net.LocalConnetion->SetConnetionType(ESimpleConnetionType::CONNETION_MAIN_LISTEN);
	Net.LocalConnetion->SetManage(this);
}

void FSimpleTCPManage::Run()
{
	while(!bEndThread)
	{
		Listen();
	}
}

void FSimpleTCPManage::FCache::Reset()
{
	TotalSize = 0;
	Cache.Empty();
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif
