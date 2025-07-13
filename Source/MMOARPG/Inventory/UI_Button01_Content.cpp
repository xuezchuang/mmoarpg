// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Button01_Content.h"
#include "Engine/FontFace.h"
#include "UMG/Public/Components/TextBlock.h"
#include "Engine/Font.h"

void UUI_Button01_Content::NativeConstruct()	
{
	Super::NativeConstruct();
}

void UUI_Button01_Content::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_Button01_Content::UpdateFontInfo(UFontFace* FontFace,UFont* Font,int32 FontSize, FLinearColor LinearColor)
{
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.TypefaceFontName = FontFace->GetFName();
	FontInfo.FontObject = Font;
	FontInfo.Size = FontSize;
	ButtonText->SetFont(FontInfo);
	FSlateColor InColorAndOpacity(LinearColor);
	ButtonText->SetColorAndOpacity(InColorAndOpacity);
}

void UUI_Button01_Content::UpdateText(FText Text)
{
	ButtonText->SetText(Text);
}

void UUI_Button01_Content::UpdateJustification(EHorizontalAlignment Alignment)
{

}
