// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "Core/UI_SimplePopup.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

static FButtonStyle* DefaultPopupButtonStyle = nullptr;

#define LOCTEXT_NAMESPACE "UUI_SimplePopup"
UUI_SimplePopup::UUI_SimplePopup(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, SimplePopupType(ESimplePopupType::ONE)
	, Delay(10.f)
	, DelayDestroy(0)
	, bDelayDestroy(false)
	, bLockView(true)
{
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	}

	if (DefaultPopupButtonStyle == nullptr)
	{
		// HACK: THIS SHOULD NOT COME FROM CORESTYLE AND SHOULD INSTEAD BE DEFINED BY ENGINE TEXTURES/PROJECT SETTINGS
		DefaultPopupButtonStyle = new FButtonStyle(FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"));

		// Unlink UMG default colors from the editor settings colors.
		DefaultPopupButtonStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultPopupButtonStyle;
}

void UUI_SimplePopup::SetText(const FText &InNewText)
{
	IntroductionBox->SetText(InNewText);
}

void UUI_SimplePopup::EnableInput()
{
	GetWorld()->GetFirstPlayerController()->EnableInput(NULL);
}

void UUI_SimplePopup::DisableInput()
{
	GetWorld()->GetFirstPlayerController()->DisableInput(NULL);
}

void UUI_SimplePopup::InitPopup(
	const FText& SureButtonNameText,
	const FText& CancelButtonNameText,
	bool bInLockView)
{
	bLockView = bInLockView;

	if (bInLockView)
	{
		DisableInput();
	}

	FText SureButtonName = SureButtonNameText;
	FText CancelButtonName = CancelButtonNameText;

	if (SureButtonName.IsEmpty())
	{
		SureButtonName = LOCTEXT("SureButton", "Sure");
	}
	if (CancelButtonName.IsEmpty())
	{
		CancelButtonName = LOCTEXT("CancelButton", "Cancel");
	}

	auto SpawnButton = [&](
		const FText &ButtonName,
		FOnButtonClickedEvent ClickedEvent,
		EHorizontalAlignment HorizontalAlignment,
		EVerticalAlignment VerticalAlignment,
		FSlateChildSize SlateChildSize,
		FMargin InPadding = FMargin())
	{
		if (UButton *NewButton = NewObject<UButton>(this, UButton::StaticClass()))
		{
			//设置button样式
			NewButton->SetStyle(WidgetStyle);

			NewButton->OnClicked = ClickedEvent;
			if (UHorizontalBoxSlot* HorizontalBoxSlot = ButtonListBox->AddChildToHorizontalBox(NewButton))
			{
				HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
				HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
				HorizontalBoxSlot->SetPadding(InPadding);
				HorizontalBoxSlot->SetSize(SlateChildSize);
			}

			if (UTextBlock *TextBlock = NewObject<UTextBlock>(this, UTextBlock::StaticClass()))
			{
				//设置字体样式
				TextBlock->SetFont(Font);

				TextBlock->SetText(ButtonName);
				NewButton->AddChild(TextBlock);
			}
		}
	};

	switch (SimplePopupType)
	{
		case ESimplePopupType::NONE:
			RemoveFromParent();
			break;
		case ESimplePopupType::DELAY:
			break;
		case ESimplePopupType::WAIT:
			break;
		case ESimplePopupType::ONE:
		{
			FOnButtonClickedEvent OnClicked;
			OnClicked.AddDynamic(this, &UUI_SimplePopup::Sure);

			SpawnButton(
				SureButtonName,
				OnClicked,
				EHorizontalAlignment::HAlign_Center,
				EVerticalAlignment::VAlign_Bottom,
				ESlateSizeRule::Fill);
			break;
		}
		case ESimplePopupType::TWO:
		{
			FOnButtonClickedEvent OnClickedSure;
			OnClickedSure.AddDynamic(this, &UUI_SimplePopup::Sure);
			FOnButtonClickedEvent OnClickedCancel;
			OnClickedCancel.AddDynamic(this, &UUI_SimplePopup::Cancel);

			SpawnButton(
				CancelButtonName,
				OnClickedCancel,
				EHorizontalAlignment::HAlign_Left,
				EVerticalAlignment::VAlign_Bottom,
				ESlateSizeRule::Automatic);

			SpawnButton(
				SureButtonName,
				OnClickedSure,
				EHorizontalAlignment::HAlign_Right,
				EVerticalAlignment::VAlign_Bottom,
				ESlateSizeRule::Fill);
			break;
		}
	}
}

void UUI_SimplePopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (SimplePopupType == ESimplePopupType::DELAY)
	{
		Delay -= InDeltaTime;
		if (Delay <=0)
		{
			Cancel();
			Delay = 100.f;
		}
	}

	if (bDelayDestroy)
	{
		DelayDestroy -= InDeltaTime;
		if (DelayDestroy <=0.f)
		{
			bDelayDestroy = false;
			RemoveFromParent();
		}
	}
}

void UUI_SimplePopup::Cancel()
{
	SimpleBlueprintCancelDelegate.ExecuteIfBound();
	SimpleCancelDelegate.ExecuteIfBound();

	if (bLockView)
	{
		EnableInput();
	}

	K2_Cancel();

	DestroyWidget();
}

void UUI_SimplePopup::Sure()
{
	SimpleBlueprintSureDelegate.ExecuteIfBound();
	SimpleSureDelegate.ExecuteIfBound();

	if (bLockView)
	{
		EnableInput();
	}

	K2_Sure();

	DestroyWidget();
}

void UUI_SimplePopup::DestroyWidget()
{
	if (DelayDestroy > 0.0f)
	{
		bDelayDestroy = true;
	}
	else
	{
		RemoveFromParent();
	}
}

#undef LOCTEXT_NAMESPACE