// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_MainBase.h"
#include "MMOARPGMacroType.h"
#include "UI_InGame.generated.h"

class UTextBlock;
class UUI_ActiveWP;
struct FMMOARPGUserData;
/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_InGame : public UUI_MainBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUI_ActiveWP* PlayerProfile;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void InitHotkeys(const TMap<FKey, int32>& Hotkeys, const TArray<int32>& Slots);
protected:

	TArray<uint32> InterestingProtos;
	TArray<FDelegateHandle> InterestingHandles;

	void RecvProtocol(uint32 ProtocolNumber);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

protected:
	

	UFUNCTION(BlueprintImplementableEvent, Category = "Status")
	void UpdateState();


};
