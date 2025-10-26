// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
//#include "SimpleNetChannelType.h"
#include "MMOARPGType.h"
#include "UI_MainBase.generated.h"

class UUI_Print;

/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_MainBase : public UUI_Base
{
	GENERATED_BODY()

	//UPROPERTY(meta = (BindWidget))
	//UUI_Print* UI_Print;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void LinkInit(){}
public:
	//void PrintLog(const FString& InMsg);
	//void PrintLog(const FText& InMsg);

protected:
	void BindClientRcv();

	virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);
	
	void LinkServer(const FSimpleAddr &InAddr);
	void LinkServer();
private:
	FDelegateHandle RecvDelegate;
};
