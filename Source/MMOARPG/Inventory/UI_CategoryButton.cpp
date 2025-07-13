// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CategoryButton.h"
#include "Components/Button.h"

void UUI_CategoryButton::TestButton()
{
	int a = 0;
}

void UUI_CategoryButton::NativeConstruct()
{
	Super::NativeConstruct();
	//Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::TestButton);
	Button_Weapons->OnClicked.__Internal_AddDynamic(this, &ThisClass::TestButton, TEXT("TestButton"));
}

void UUI_CategoryButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CategoryButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	//Preview_Icon->SetBrushResourceObject(CategoryIcon);
	//Preview_Icon->SetColorAndOpacity(IconColor);
}
