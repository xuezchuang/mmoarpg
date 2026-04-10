// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CategoryButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"

static const FLinearColor GIconColorActive   = FLinearColor(FColor::FromHex(TEXT("414758FF")));
static const FLinearColor GIconColorInactive = FLinearColor(FColor::FromHex(TEXT("EDE5DCFF"))); 
static const FLinearColor GBorderColorActive   = FLinearColor(FColor::FromHex(TEXT("00000000"))); 
static const FLinearColor GBorderColorInactive = FLinearColor(FColor::FromHex(TEXT("FFFFFFFF")));

void UUI_CategoryButton::SetActive(bool bActive)
{
	if (Image_92)
	{
		Image_92->SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (Button_Preview_Icon)
	{
		Button_Preview_Icon->SetColorAndOpacity(bActive ? GIconColorActive : GIconColorInactive);
	}
	if (Border_Weapons)
	{
		Border_Weapons->SetBrushColor(bActive ? GBorderColorActive : GBorderColorInactive);
	}
}

void UUI_CategoryButton::NativeConstruct()
{
	Super::NativeConstruct();
	SetActive(false);
}

void UUI_CategoryButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CategoryButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Button_Preview_Icon && Texture)
	{
		Button_Preview_Icon->SetBrushResourceObject(Texture);
	}
	SetActive(false);
}
