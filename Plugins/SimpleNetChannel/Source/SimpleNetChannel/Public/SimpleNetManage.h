#pragma once

#include "SimpleNetChannelType.h"
#include "SimpleProtocolsDefinition.h"

#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif
//#include "UObject/SimpleController.h"

DECLARE_DELEGATE_TwoParams(FSimpleNetManageMsgDelegate,ESimpleNetErrorType,const FString&);


class FSimpleConnetion;
class FInternetAddr;
class USimpleNetworkObject;
class FSimpleChannel;
class USimpleController;

class SIMPLENETCHANNEL_API FSimpleNetManage
#ifdef PLATFORM_PROJECT
	:public FTickableGameObject
#endif
{
public:
	FSimpleNetManageMsgDelegate NetManageMsgDelegate;

	UClass* NetworkObjectClass;
	UClass* SimplePlayerClass;
public:
	void LinkTimeout(const FString &InTimeoutMsg);
public:
	FSimpleNetManage();
	virtual ~FSimpleNetManage(){}

	static FSimpleNetManage *CreateManage(ESimpleNetLinkState InState, ESimpleSocketType InType);
	static void Destroy(FSimpleNetManage *InNetInstance);

	virtual bool Init(int32 InPort = 0);
	virtual bool Init(uint32 InIP,uint32 InPort);
	virtual bool Init(const FSimpleAddr& InAddr);
	virtual bool Init(const FString& InIP, uint32 InPort);

	virtual void Tick(float DeltaTime);
	virtual void Close();
	virtual void Close(const FSimpleAddrInfo &InCloseConnetion);
	virtual void Close(const TSharedPtr<FInternetAddr> &InternetAddr);

	USimpleController* GetController();
	USimpleNetworkObject* GetNetworkObject(uint32 InIP,uint32 InPort,const FGuid &InChannelID);

	FSimpleChannel* GetChannel();
	FSimpleChannel* GetChannel(uint32 InIP, uint32 InPort, const FGuid& InChannelID);

	int32 GetConnetionNum();
	FSimpleAddr GetAddr();

	static FString GetAddrString(const FSimpleAddr& InAddr);
	static FSimpleAddr GetSimpleAddr(const TCHAR* InIP,uint32 InPort);
	static void GetLocalIPAndPort(FString &InIP,uint32 &Port);

	//该API使用需要保证你当前的网络管理是服务器 它会遍历服务器的远端链接对象
	template<class T>
	void FindRemoteNetworkObjectByLambda(TFunction<ESimpleNetManageCallType(T*)> InImplement);
protected:
	virtual bool CloseSocket();
public:
	static USimpleNetworkObject *GetNetManageNetworkObject(FSimpleNetManage* InSimpleNetManage, const FSimpleAddrInfo& AddrInfo);

public:
	FORCEINLINE ESimpleNetLinkState GetLinkState() { return LinkState; }

protected:
	virtual TStatId GetStatId()const;
	virtual void VerificationConnectionInfo(TSharedPtr<FSimpleConnetion> InConnetion, uint8* InData, int32 InByteNumber);
protected:
	ESimpleNetLinkState LinkState;

	struct FNet
	{
		bool IsAddr(TSharedPtr<FInternetAddr> InternetAddr);
		int32 Add(TSharedPtr<FInternetAddr> InternetAddr);
		void Clear(int32 InIndex);
		TSharedPtr<FSimpleConnetion> GetEmptyConnetion(TSharedPtr<FInternetAddr> InternetAddr);
	public:
		TSharedPtr<FSimpleConnetion> operator[](TSharedPtr<FInternetAddr> InternetAddr);
		TSharedPtr<FSimpleConnetion> operator[](const FSimpleAddr &InternetAddr);
	public:
		TSharedPtr<FSimpleConnetion> LocalConnetion;
		TArray<TSharedPtr<FSimpleConnetion>> RemoteConnetions;
	}Net;

	bool bInit;
};



#include "Channel/SimpleChannel.h"

class USimpleNetworkObject;
template<class T>
void FSimpleNetManage::FindRemoteNetworkObjectByLambda(TFunction<ESimpleNetManageCallType(T*)> InImplement)
{
	for (auto& Tmp : Net.RemoteConnetions)
	{
		ESimpleNetManageCallType SimpleNetManageCallType = ESimpleNetManageCallType::INPROGRESS;
		for (auto& TmpChannel : Tmp->GetChannels())
		{
			if (TmpChannel.IsValid())
			{
				SimpleNetManageCallType = InImplement(TmpChannel.GetNetObject<T>());
				if (SimpleNetManageCallType == ESimpleNetManageCallType::PROGRESS_COMPLETE)
				{
					break;
				}	
			}
		}

		if (SimpleNetManageCallType == ESimpleNetManageCallType::PROGRESS_COMPLETE)
		{
			break;
		}
	}
}