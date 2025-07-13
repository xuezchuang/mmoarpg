#pragma once

#include "CoreMinimal.h"
//#include "SimpleNetManage.h"

class FMMOARPGGateRobot
{
public:
	FSimpleDelegate StartDelegate;

	FMMOARPGGateRobot();
	virtual ~FMMOARPGGateRobot();

	//这个协议主要是为了在网关那边注册id
	void RunCharacterAppearanceRequests();
	//这个协议主要是为了登录 在服务器注册一个玩家
	void RunLoginToDSServerRequests();

	void Init(const FString& Host, const int32 Port);

	virtual void Tick(float DeltaTime);

	//virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

private:
	//FSimpleNetManage* GateClient;
};
