// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimplePopupType.h"
#include "UI_SimplePopup.generated.h"


class UMultiLineEditableTextBox;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class SIMPLEPOPUP_API UUI_SimplePopup : public UUserWidget
{
	GENERATED_BODY()

	friend struct FPopupHandle;

	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* IntroductionBox;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* ButtonListBox;
public:
	UPROPERTY(BlueprintReadOnly, Category = "Popup")
	ESimplePopupType SimplePopupType;

	/** The button style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button", meta = (DisplayName = "Style"))
	FButtonStyle WidgetStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button", meta = (DisplayName = "Font"))
	FSlateFontInfo Font;

	UPROPERTY()
	float Delay;

	//当点击确定或者取消会延迟删除该对象，该功能专门为结束后播放动画使用
	UPROPERTY()
	float DelayDestroy;

	UPROPERTY()
	bool bDelayDestroy;

	UPROPERTY()
	FSimpleBlueprintDelegate SimpleBlueprintSureDelegate;
	FSimpleDelegate SimpleSureDelegate;

	UPROPERTY()
	FSimpleBlueprintDelegate SimpleBlueprintCancelDelegate;
	FSimpleDelegate SimpleCancelDelegate;
public:
	UUI_SimplePopup(const FObjectInitializer& ObjectInitializer);

	void SetText(const FText &InNewText);

	void InitPopup(const FText& SureButtonNameText,const FText& CancelButtonNameText,bool bInLockView = true);

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "Sure"))
	void K2_Sure();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "Cancel"))
	void K2_Cancel();
protected:
	UFUNCTION()
	void Cancel();

	UFUNCTION()
	void Sure();

	void DestroyWidget();
protected:
	void EnableInput();
	void DisableInput();
protected:
	UPROPERTY()
	bool bLockView;
};
