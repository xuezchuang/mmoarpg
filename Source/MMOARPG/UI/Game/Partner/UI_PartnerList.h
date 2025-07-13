// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_MainBase.h"
#include "UI_PartnerList.generated.h"

class UVerticalBox;
class UUI_PartnerInfo;

/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_PartnerList : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CharacterList;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_PartnerInfo> UI_PartnerInfoClass;
public:

	void Init();

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void CallCharacterListByPredicate(TFunction<bool(UUI_PartnerInfo* Info)> InFun);
};
