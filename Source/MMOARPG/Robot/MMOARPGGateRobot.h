#pragma once

#include "CoreMinimal.h"
//#include "SimpleNetManage.h"

class FMMOARPGGateRobot
{
public:
	FSimpleDelegate StartDelegate;

	FMMOARPGGateRobot();
	virtual ~FMMOARPGGateRobot();

	//���Э����Ҫ��Ϊ���������Ǳ�ע��id
	void RunCharacterAppearanceRequests();
	//���Э����Ҫ��Ϊ�˵�¼ �ڷ�����ע��һ�����
	void RunLoginToDSServerRequests();

	void Init(const FString& Host, const int32 Port);

	virtual void Tick(float DeltaTime);

	//virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

private:
	//FSimpleNetManage* GateClient;
};
