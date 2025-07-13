//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_ComboCount.generated.h"

class UTextBlock;
class UProgressBar;
/**
 *
 */
UCLASS()
class SIMPLECOMBAT_API UUI_ComboCount : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ComboText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ComboAttackBar;
public:
	UUI_ComboCount(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void SetComboText(const int32 Num);

	void PlayAnim();

	void SetMaxDeltaTime(float InNewMaxDeltaTime);

	void Die();
protected:
	UWidgetAnimation* GetNameWidgetAnimation(const FString& InWidgetAnimName);
	void PlayWidgetAnim(const FString& InWidgetAnimName);

protected:
	float TimeDelta;
	float MaxDeltaTime;
	FTimerHandle DieDelayTimeHandle;
};
