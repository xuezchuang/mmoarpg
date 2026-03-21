// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CategoryTabButton.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUI_CategoryTabButton::Active(bool InbActive)
{
	StopAnim(TEXT("OnHover"));
	
	bActive = InbActive;
	if(bActive)
	{
		Icon_Texture->SetColorAndOpacity(Icon_Active_Color);
		ActiveLine->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HoverPulse->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FSlateBrush brush = Border_background->Background;
		brush.DrawAs = ESlateBrushDrawType::Image;
		brush.TintColor = Border_Active_Color;
		Border_background->SetBrush(brush);
		Border_background->SetRenderScale(FVector2D(1.0, 1.0));
		TextGuide->SetRenderOpacity(0.0f);
		
		PlayWidgetAnim(TEXT("PulseLoop"), EUMGSequencePlayMode::Forward, 0);
		//SetKeyboardFocus();
	}
	else
	{
		Icon_Texture->SetColorAndOpacity(Icon_NActive_Color);
		ActiveLine->SetVisibility(ESlateVisibility::Collapsed);
		HoverPulse->SetVisibility(ESlateVisibility::Collapsed);
		FSlateBrush brush = Border_background->Background;
		brush.DrawAs = ESlateBrushDrawType::NoDrawType;
		brush.TintColor = Border_NActive_Color;
		Border_background->SetBrush(brush);
		StopAnim(TEXT("PulseLoop"));
	}
}

void UUI_CategoryTabButton::OnButtonClicked()
{
	FunClicked.ExecuteIfBound(CurrentType);
}

void UUI_CategoryTabButton::OnHover()
{
	if(bActive)
		return;

	ActiveLine->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Icon_Texture->SetColorAndOpacity(Icon_Hover_Color);
	PlayWidgetAnim(TEXT("OnHover"));
}

void UUI_CategoryTabButton::OnUnHover()
{
	if(bActive)
		return;

	ActiveLine->SetVisibility(ESlateVisibility::Collapsed);
	Icon_Texture->SetColorAndOpacity(Icon_NActive_Color);
	PlayWidgetAnim(TEXT("OnHover"), EUMGSequencePlayMode::Reverse);
}

void UUI_CategoryTabButton::NativeConstruct()
{
	Super::NativeConstruct();
	Active(bActive);
	//CT_Button->OnHovered.__Internal_AddDynamic(this, &UUI_CategoryTabButton::OnHover, FName(Text("OnHover")));
	CT_Button->OnHovered.__Internal_AddDynamic(this, &UUI_CategoryTabButton::OnHover, TEXT("OnHover"));
	CT_Button->OnUnhovered.__Internal_AddDynamic(this, &UUI_CategoryTabButton::OnUnHover, TEXT("OnUnHover"));
	CT_Button->OnClicked.__Internal_AddDynamic(this, &UUI_CategoryTabButton::OnButtonClicked, TEXT("OnButtonClicked"));
}

void UUI_CategoryTabButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CategoryTabButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	Icon_Texture->SetBrushResourceObject(CategoryIcon);
	TextGuide->SetText(Text);
	//OnUnHover();
}
