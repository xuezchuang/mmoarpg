// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_WindowSwitcher.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
//#include "UI_LoginMain.h"
//#include "Stream/SimpleIOStream.h"
#include "UI_CategoryTabButton.h"
#include <Components/WidgetSwitcher.h>
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUI_WindowSwitcher::NativeConstruct()
{
	Super::NativeConstruct(); 
	WB_Category_Inventory->FunClicked.BindUObject(this,		&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Quest->FunClicked.BindUObject(this,		&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Map->FunClicked.BindUObject(this,			&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Skill->FunClicked.BindUObject(this,		&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Rune->FunClicked.BindUObject(this,		&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Glossary->FunClicked.BindUObject(this,			&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Gallery->FunClicked.BindUObject(this,			&UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Guide->FunClicked.BindUObject(this,		&UUI_WindowSwitcher::SetNativeUIType);

	WB_Category_Vendor_Buy->FunClicked.BindUObject(this, &UUI_WindowSwitcher::SetNativeUIType);
	WB_Category_Player_Sell->FunClicked.BindUObject(this, &UUI_WindowSwitcher::SetNativeUIType);
	//WB_Category_Storage
	//SetFocus();
}

void UUI_WindowSwitcher::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_WindowSwitcher::NativePreConstruct()
{
	Super::NativePreConstruct();
	//SetNativeUIType()
}

void UUI_WindowSwitcher::InitOutIcon(UImage* InIconTexture,UTextBlock* InActiveTextTab,E_UpSwitchType InActiveTabType)
{
	ActiveTabType = InActiveTabType;
	IconTexture = InIconTexture;
	ActiveTextTab = InActiveTextTab;
	switch (ActiveTabType)
	{
		case E_UpSwitchType::None:
			break;
		case E_UpSwitchType::MainTab:
			Switcher->SetActiveWidget(MainTab);
			SetType(E_UIType::Character);
			break;
		case E_UpSwitchType::Vendor:
			Switcher->SetActiveWidget(Vendor);
			SetType(E_UIType::Buy);
			break;
		case E_UpSwitchType::Storage:
			Switcher->SetActiveWidget(Storage);
			break;
		case E_UpSwitchType::LoadGame:
			Switcher->SetActiveWidget(LoadGame);
			break;
		default:
			break;
	}
}

void UUI_WindowSwitcher::SetType(E_UIType eType,bool bPassivity /* = false */)
{
	if (eType == m_eCurType)
		return;

	UUI_CategoryTabButton* pWidget = GetUIButtonByType(m_eCurType);
	if (pWidget)
	{
		pWidget->Active(false);
	}
	pWidget = GetUIButtonByType(eType);
	if (pWidget)
	{
		pWidget->Active(true);
		IconTexture->SetBrushFromTexture(pWidget->CategoryIcon);
	}
	m_eCurType = eType;

	

	switch (eType)
	{
		case E_UIType::None:
			break;
		case E_UIType::Character:
			
			break;
		case E_UIType::Joural:
			break;
		case E_UIType::Map:
			break;
		case E_UIType::Ability:
			break;
		case E_UIType::Crafting:
			break;
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
		case E_UIType::Buy:
			ActiveTextTab->SetText(FText::FromString("Food Shop/Buy"));
			break;
		case E_UIType::SellBack:
			ActiveTextTab->SetText(FText::FromString("Food Shop/Sell Back"));
			break;
		default:
			break;
	}
}

void UUI_WindowSwitcher::SetNativeUIType(E_UIType eType)
{
	SetType(eType, true);
	WindowSwitchDelegate.ExecuteIfBound(eType);
}

UUI_CategoryTabButton* UUI_WindowSwitcher::GetUIButtonByType(E_UIType eType)
{
	UUI_CategoryTabButton* pButton = NULL;
	switch (eType)
	{
	case E_UIType::None:
		break;
	case E_UIType::Character:
		if (ActiveTabType == E_UpSwitchType::Vendor)
		{
			pButton = WB_Category_Inventory;
		}
		else
		{
			pButton = WB_Category_Player_Sell;
		}
		break;
	case E_UIType::Joural:
		pButton = WB_Category_Quest;
		break;
	case E_UIType::Map:
		pButton = WB_Category_Map;
		break;
	case E_UIType::Ability:
		pButton = WB_Category_Skill;
		break;
	//case E_UIType::Crafting:
	//	pButton = WB_Button01_Crafting;
	//	break;
	case E_UIType::Runes:
		pButton = WB_Category_Rune;
		break;
	//case E_UIType::CodeX:
	//	pButton = WB_Category_Glossary;
	//	break;
	case E_UIType::Glossary:
		pButton = WB_Category_Glossary;
		break;
	case E_UIType::Gallery:
		pButton = WB_Category_Gallery;
		break;
	case E_UIType::Control:
		pButton = WB_Category_Guide;
		break;
	case E_UIType::Buy:
		pButton = WB_Category_Vendor_Buy;
		break;
	case E_UIType::SellBack:
		pButton = WB_Category_Player_Sell;
		break;
	default:
		break;
	}
	return pButton;
}

