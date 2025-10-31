// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_MainBase.h"
#include "MMOARPGMacroType.h"
#include "UI_InGame.generated.h"

class UTextBlock;
class UUI_ActiveWP;
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
	
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	double m_dCurHP;

	UPROPERTY(BlueprintReadOnly, Category = "Status")
	double m_dTolHP;

	UPROPERTY(BlueprintReadOnly, Category = "Status")
	double m_dCurMP;

	UPROPERTY(BlueprintReadOnly, Category = "Status")
	double m_dTolMP;

	UFUNCTION(BlueprintImplementableEvent, Category = "Status")
	void UpdateState();
};
