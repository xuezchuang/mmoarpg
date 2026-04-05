// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_MainBase.h"
#include "MMOARPG.h"
#include "UI_Print.h"
#include "ThreadManage.h"
#include "UObject/SimpleController.h"
#include "MMOARPGMacroType.h"


void UUI_MainBase::NativeConstruct()
{
	Super::NativeConstruct();
	//UE_LOG(MMOARPG, Display, TEXT("UUI_MainBase::NativeConstruct"));
}

void UUI_MainBase::NativeDestruct()
{
	Super::NativeDestruct();

	if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		if(InGameInstance->GetClient() && InGameInstance->GetClient()->GetController())
		{
			InGameInstance->GetClient()->GetController()->RecvDelegate.Remove(RecvDelegate);
		}
	}
}

//void UUI_MainBase::PrintLog(const FString& InMsg)
//{
//	PrintLog(FText::FromString(InMsg));
//}
//
//void UUI_MainBase::PrintLog(const FText& InMsg)
//{
//	//播放动画
//	UI_Print->PlayTextAnim();
//
//	UI_Print->SetText(InMsg);
//}


