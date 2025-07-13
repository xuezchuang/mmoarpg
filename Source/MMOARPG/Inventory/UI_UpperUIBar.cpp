// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_UpperUIBar.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
//#include "UI_LoginMain.h"
//#include "Stream/SimpleIOStream.h"
#include "GameFramework/PlayerController.h"
#include "UI_CraftingWindow.h"
#include "UI_WindowSwitcher.h"

void UUI_UpperUIBar::NativeConstruct()
{
	Super::NativeConstruct(); 
	//WB_WindowSwitcher->WindowSwitchDelegate.BindUObject(this, &UUI_UpperUIBar::ShowWidget);
	Button_Close->OnClicked.AddDynamic(this, &UUI_UpperUIBar::OnClosed);
	
	//m_eCurType = E_UIType::None;
	//Widget_QuestLog->SetVisibility(ESlateVisibility::Hidden);
	//WB_Inventory_Overall->SetVisibility(ESlateVisibility::Hidden);
	//WB_CraftingWindow->SetVisibility(ESlateVisibility::Hidden);
	//SignInButton->OnReleased.AddDynamic(this,&UpperUIBar::SignIn);
	//RegisterButton->OnReleased.AddDynamic(this, &UpperUIBar::Register);
}

void UUI_UpperUIBar::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_UpperUIBar::ShowWidget(E_UIType eType)
{
	//if (!m_bVisibility)
	//{
	//	if (APlayerController* InPlayerController = GetPlayerController<APlayerController>())
	//	{
	//		//InPlayerController->SetPause(true);
	//		//InPlayerController->SetShowMouseCursor(true);
	//		//FInputModeUIOnly InputMode;
	//		//InputMode.SetWidgetToFocus(TakeWidget());
	//		//InPlayerController->SetInputMode(InputMode);
	//		//InPlayerController->FlushPressedKeys();
	//		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("SetInputMode"));
	//	}
	//}
	////FString EnumName = StaticEnum<E_UIType>()->GetNameStringByValue(static_cast<int32>(eType));
	////GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, EnumName);
	//SetVisibility(ESlateVisibility::Visible);
	//if (eType == m_eCurType)
	//	return;
	//UUserWidget* pWidget = GetWidgetByType(m_eCurType);
	//if(pWidget)
	//	pWidget->SetVisibility(ESlateVisibility::Hidden);
	//pWidget = GetWidgetByType(eType);
	//if (pWidget)
	//	pWidget->SetVisibility(ESlateVisibility::Visible);
	//m_eCurType = eType;
	//WB_WindowSwitcher->SetType(eType);



}

UUserWidget* UUI_UpperUIBar::GetWidgetByType(E_UIType eType)
{
	UUserWidget* pWidget = NULL;
	switch (eType)
	{
	case E_UIType::None:
		break;
	//case E_UIType::Character:
	//	pWidget = WB_Inventory_Overall;
	//	break;
	//case E_UIType::Joural:
	//	pWidget = Widget_QuestLog;
	//	break;
	case E_UIType::Map:
		break;
	case E_UIType::Ability:
		break;
	//case E_UIType::Crafting:
	//	pWidget = WB_CraftingWindow;
	//	break;
	case E_UIType::Runes:
		break;
	case E_UIType::CodeX:
		break;
	case E_UIType::Glossary:
		break;
	case E_UIType::Gallery:
		break;
	case E_UIType::Control:
		break;
	default:
		break;
	}
	return pWidget;
}

void UUI_UpperUIBar::OnClosed()
{
	if (APlayerController* InPlayerController = GetPlayerController<APlayerController>())
	{
		//FInputModeGameOnly InputMode;
		//InPlayerController->SetInputMode(InputMode);
		//InPlayerController->SetPause(false);
		//InPlayerController->SetShowMouseCursor(true);
	}
	SetVisibility(ESlateVisibility::Hidden);
	m_bVisibility = false;
}

