// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Button01_FX.h"
#include <HAL/Platform.h>

void UUI_Button01_FX::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_Button01_FX::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_Button01_FX::OnClicked()
{
	UWidgetAnimation* pAnimation1 = GetNameWidgetAnimation(TEXT("OnClicked_FX01"));
	UWidgetAnimation* pAnimation2 = GetNameWidgetAnimation(TEXT("OnClicked_FX02"));
	PlayAnimation(pAnimation1, 0.0f, 1, EUMGSequencePlayMode::Forward, 4.0);
	PlayAnimation(pAnimation2, 0.0f, 1, EUMGSequencePlayMode::Reverse, 12.0);
	if(!bIsClicked)
	{
		bIsClicked = true;
		UWidgetAnimation* pAnimation3 = GetNameWidgetAnimation(TEXT("OnHovered_FadeAnim"));
		PlayAnimation(pAnimation3, 0.0f, 1, EUMGSequencePlayMode::Reverse, 12.0);
	}
}

void UUI_Button01_FX::UnClicked()
{
	if(bIsClicked)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Button01_FX UnClicked Error")));
		return;
	}
	bIsClicked = false;
	UWidgetAnimation* pAnimation = GetNameWidgetAnimation(TEXT("OnHovered_FadeAnim"));
	PlayAnimation(pAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0);
}
