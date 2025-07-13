// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimplePopupType.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSimpleBlueprintDelegate);

UENUM(BlueprintType)
enum class ESimplePopupType :uint8
{
	NONE  UMETA(DisplayName = "None"),//空的
	DELAY UMETA(DisplayName = "Delay"), //给定一个时间，到了就删除自己
	WAIT  UMETA(DisplayName = "Wait"),//弹窗会一直出现
	ONE   UMETA(DisplayName = "Once Button"),//会生成一个按钮 这个按钮绑定取消
	TWO   UMETA(DisplayName = "Two Button"),//出现两个一个绑定取消 一个绑定确定
};

class UUI_SimplePopup;
USTRUCT(BlueprintType)
struct FPopupHandle
{
	GENERATED_USTRUCT_BODY()

	SIMPLEPOPUP_API bool Sure();
	SIMPLEPOPUP_API bool Cancel();

	void SetSimplePopup( UUI_SimplePopup *NewSimplePopup);
private:
	TWeakObjectPtr<UUI_SimplePopup> SimplePopup;
};

