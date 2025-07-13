#include "SimpleTCPConnetion.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "../Log/SimpleNetChannelLog.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "Channel/SimpleChannel.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

void FSimpleTCPConnetion::ConnectVerification()
{
	if(Socket->Connect(*LocalAddr))
	{
		UE_LOG(LogSimpleNetChannel, Display, TEXT("Connect Success [IP:%s Port:%i] [local Port:%d]"), *LocalAddr->ToString(false), LocalAddr->GetPort(), Socket->GetPortNo());
		SetState(ESimpleConnetionLinkType::LINK_Connect);
		Super::ConnectVerification();
	}
}

void FSimpleTCPConnetion::Analysis(uint8* InData, int32 BytesNumber)
{
	Super::Analysis(InData, BytesNumber);
	//char* String = (char*)InData;

//	UE_LOG(LogSimpleNetChannel, Display, TEXT("Analysis Data = [%s],Number = [%i]"), UTF8_TO_TCHAR(String),BytesNumber);
}

void FSimpleTCPConnetion::Send(TArray<uint8>& InData)
{
	FSimpleConnetion::Send(InData);

	if(Socket)
	{
		int32 BytesSend = 0;
		if(Socket->Send(InData.GetData(), InData.Num(), BytesSend))
		{
			if(FSimpleNetGlobalInfo::Get()->GetInfo().bShowSendDebug)
			{
				UE_LOG(LogSimpleNetChannel, Display, TEXT("Send %i Bytes"), BytesSend);
			}
		}
		else
		{
			int32 LastError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
			UE_LOG(LogSimpleNetChannel, Error, TEXT("%s"), *FString::Printf(TEXT("Send Error!Please check whether the peer address is correct:%d"), LastError));
		}
	}
}

void FSimpleTCPConnetion::Receive(const FGuid& InChannelID, TArray<uint8>& InData)
{
	FSimpleConnetion::Receive(InChannelID, InData);
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif