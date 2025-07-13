// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_MainBase.h"
#include "UI_PartnerInfo.generated.h"

class UCheckBox;
class UImage;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_PartnerInfo : public UUI_Base
{
	GENERATED_BODY()

	friend class UUI_PartnerList;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* ClickedCheckBox;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

public:
	UUI_PartnerInfo(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void ShowSelected(bool bShow);
public:

	UFUNCTION()
	void OnClickedCharacter(bool bClicked);
public:
	int32 GetCharacterID() { return CharacterID; }
protected:

	void SetCharacterID(int32 InCharacterID) { CharacterID=InCharacterID; }
protected:
	int32 CharacterID;
};
