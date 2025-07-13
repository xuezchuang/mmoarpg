// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingListSpacer.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/TextBlock.h"

void UUI_CraftingListSpacer::NativeConstruct()
{
	Super::NativeConstruct(); 
	Native_Button->OnClicked.AddDynamic(this, &UUI_CraftingListSpacer::OnClicked);
	Native_Button->OnHovered.AddDynamic(this, &UUI_CraftingListSpacer::OnHovered);
	Native_Button->OnUnhovered.AddDynamic(this, &UUI_CraftingListSpacer::OnUnHovered);
}

void UUI_CraftingListSpacer::NativeDestruct()
{
	Super::NativeDestruct();
}

UWidget* UUI_CraftingListSpacer::GetBox()
{
	return AssociatedBox;
}

void UUI_CraftingListSpacer::OnClicked()
{
	if(bFlip)
	{
		Triangle->SetRenderTransformAngle(90.0f);
		if (AssociatedBox)
			AssociatedBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Triangle->SetRenderTransformAngle(0.0f);
		if (AssociatedBox)
			AssociatedBox->SetVisibility(ESlateVisibility::Visible);
	}
	bFlip = !bFlip;
}
void UUI_CraftingListSpacer::OnHovered()
{
	//FString FColor = FString::Printf(TEXT("OnHovered:%X%X%X%X"), BG_Hovered_Color.R, BG_Hovered_Color.G, BG_Hovered_Color.B, BG_Hovered_Color.A);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FColor);
	BG_Image->SetColorAndOpacity(BG_Hovered_Color);
}
void UUI_CraftingListSpacer::OnUnHovered()
{
	//FString FColor = FString::Printf(TEXT("OnUnHovered%X%X%X%X"), BG_UnHovered_Color.R, BG_UnHovered_Color.G, BG_UnHovered_Color.B, BG_UnHovered_Color.A);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FColor);
	BG_Image->SetColorAndOpacity(BG_UnHovered_Color);
}

void UUI_CraftingListSpacer::NativePreConstruct()
{
	Super::NativePreConstruct();
	TextBlock_Title->SetText(Title); 
}

