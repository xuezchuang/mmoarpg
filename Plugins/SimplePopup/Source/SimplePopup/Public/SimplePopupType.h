// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimplePopupType.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSimpleBlueprintDelegate);

UENUM(BlueprintType)
enum class ESimplePopupType :uint8
{
	NONE  UMETA(DisplayName = "None"),//�յ�
	DELAY UMETA(DisplayName = "Delay"), //����һ��ʱ�䣬���˾�ɾ���Լ�
	WAIT  UMETA(DisplayName = "Wait"),//������һֱ����
	ONE   UMETA(DisplayName = "Once Button"),//������һ����ť �����ť��ȡ��
	TWO   UMETA(DisplayName = "Two Button"),//��������һ����ȡ�� һ����ȷ��
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

