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

public:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UTextBlock* MountName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MountPower;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CharacterName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CharacterLevel;

	UPROPERTY(BlueprintReadOnly,Category="UI")
	FRoleBasicView RoleInfo;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool IsSelf = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Status")
	void UpdateState();

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateMountUIText(const FText& InMountNameText = FText::GetEmpty(), const FText& InMountPowerText = FText::GetEmpty());
protected:

	TArray<uint32> InterestingProtos;
	TArray<FDelegateHandle> InterestingHandles;

	void RecvProtocol(uint32 ProtocolNumber);
	void RefreshSelfRoleInfo();

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

protected:
	bool bUseForMount = false;
};
