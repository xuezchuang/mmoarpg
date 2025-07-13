// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimplePopupType.h"

class UUI_SimplePopup;

//仅仅提供C++使用
namespace SimplePopupUtils
{
	SIMPLEPOPUP_API FPopupHandle CreatePopup(
		UWorld *InWorld,
		TSubclassOf<UUI_SimplePopup> PopupClass,
		const FText &Content,
		ESimplePopupType SimplePopupType,
		const FText& SureButtonNameText,
		const FText& CancelButtonNameText,
		float Life = 10.f,
		float DelayDestroy = 0.f,
		FSimpleDelegate SureDelegate = FSimpleDelegate(),
		FSimpleDelegate CancelDelegate = FSimpleDelegate(), bool bLockView = true);
}