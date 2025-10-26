// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_MainBase.h"
#include "MMOARPGMacroType.h"
#include "UI_InGame.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_InGame : public UUI_MainBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterHP;

	UPROPERTY(meta = (BindWidget))
	UUI_ActiveWP* PlayerProfile;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
protected:

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

protected:
};
