// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Button01_Native.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
//#include "UI_LoginMain.h"
//#include "Stream/SimpleIOStream.h"
#include "UI_NativeButton.h"
#include "UI_Button01_Content.h"
#include "UMG/Public/Components/SizeBox.h"
#include "UI_Button01_FX.h"
#include "UMG/Public/Components/Border.h"
#include "UMG/Public/Components/TextBlock.h"

void UUI_Button01_Native::NativePreConstruct()
{
	Super::NativePreConstruct();

	WB_Button01_Content->UpdateText(ButtonText);
	SetFX_BaselineHeight();
	SetNativeButtonClickAreaHeight();
	
}

void UUI_Button01_Native::NativeConstruct()
{
	Super::NativeConstruct();
	WB_NativeButton->Native_Button->OnClicked.AddDynamic(this, &UUI_Button01_Native::OnButtonClicked);
	WB_NativeButton->Native_Button->OnHovered.AddDynamic(this, &UUI_Button01_Native::OnButtonHovered);
	WB_NativeButton->Native_Button->OnUnhovered.AddDynamic(this, &UUI_Button01_Native::OnButtonUnhovered);

	WB_Button01_Content->UpdateText(ButtonText);
	SetFX_BaselineHeight();
	SetNativeButtonClickAreaHeight();
	
}

void UUI_Button01_Native::NativeDestruct()
{
	Super::NativeDestruct();
}


void UUI_Button01_Native::ActiveButton(bool bActive, bool bPassivity /* = true */)
{
	m_bClicked = bActive;
	if (!bActive)
	{
		WB_Button01_Content->UpdateFontInfo(TypeFace_Normal, Font_Normal, FontSize_Normal, FontColor_Normal);
		WB_Button01_FX->UnClicked();
	}
	else if(!bPassivity)
	{
		WB_Button01_Content->UpdateFontInfo(TypeFace_Clicked, Font_Clicked, FontSize_Clicked, FontColor_Clicked);
	}
}

void UUI_Button01_Native::OnButtonClicked()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("UUI_Button01_Native ButtonClicked")));
	FunClicked.Execute(m_eCurType);
	//WB_Button01_Content->UpdateFontInfo(TypeFace_Clicked, Font_Clicked, FontSize_Clicked, FontColor_Clicked);
}
void UUI_Button01_Native::OnButtonHovered()
{
	WB_Button01_Content->UpdateFontInfo(TypeFace_Hovered, Font_Hovered, FontSize_Hovered, FontColor_Hovered);
}
void UUI_Button01_Native::OnButtonUnhovered()
{
	if(m_bClicked)
		WB_Button01_Content->UpdateFontInfo(TypeFace_Clicked, Font_Clicked, FontSize_Clicked, FontColor_Clicked);
	else
		WB_Button01_Content->UpdateFontInfo(TypeFace_Normal, Font_Normal, FontSize_Normal, FontColor_Normal);
}

void UUI_Button01_Native::SetNativeButtonClickAreaHeight()
{
	WB_Button01_Content->ButtonText->ForceLayoutPrepass();
	FVector2D ContentSize = WB_Button01_Content->ButtonText->GetDesiredSize(); 
	SB_NativeButton->SetWidthOverride(ContentSize.X + 30.0);
	SB_FX->SetWidthOverride(ContentSize.X + 30.0);
}

void UUI_Button01_Native::SetFX_BaselineHeight()
{
	if(m_bDoOnce)
	{
		FVector2D Translation = SB_FX->GetRenderTransform().Translation;
		FVector2D Vec2 = WB_Button01_FX->b_Content_Clicked->GetRenderTransform().Translation;
		FVector2D SBFXTrans;
		SBFXTrans.X = Translation.X - Vec2.X;
		SBFXTrans.Y = Translation.Y - Vec2.Y;
		SB_FX->SetRenderTranslation(SBFXTrans);
	}
	m_bDoOnce = false;
}

