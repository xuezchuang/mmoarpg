// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_MainBase.h"
#include "MMOARPGMacroType.h"
#include "UI_ActiveWP.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_ActiveWP : public UUI_MainBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MountName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MountPower;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateMountUIText(const FText& InMountNameText = FText::GetEmpty(), const FText& InMountPowerText = FText::GetEmpty());
protected:

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

protected:
	bool bUseForMount = false;
};
