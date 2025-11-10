// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_MainBase.h"
#include "MMOARPGType.h"
#include "../Core/Common/MMOARPGNetSubsystem.h"
#include "UI_LoginMain.generated.h"

class FSimpleChannel;
class UUI_Login;
class UUI_Register;
/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_LoginMain : public UUI_MainBase
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* UI_LinkWidget;

	UPROPERTY(meta = (BindWidget))
	UUI_Login* UI_Login;

	UPROPERTY(meta = (BindWidget))
	UUI_Register* UI_Register;

	UPROPERTY(meta = (BindWidget))
	UUI_Print* UI_Print;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);
	

public:
	void SignIn(FString& ,FString& );
public:
	void Register();
	void Register(FString InRegisterInfo);
	void Register(FString Name,FString Pass);
protected:
	void LinkInit(ENetServerRole ServerRole);
	void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//UFUNCTION()
	void HandleRegisterResponse(ERegistrationType Type);
};
