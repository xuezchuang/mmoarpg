#pragma once

#include "CoreMinimal.h"
//#include "SimpleNetManage.h"

class FMMOARPGDSRobot
{
public:
	FMMOARPGDSRobot();

	void Init(const FString& Host, const int32 Port);

	virtual void Tick(float DeltaTime);

private:
	//FSimpleNetManage* DSClient;
};