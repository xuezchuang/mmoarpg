#include "UI_KeybindingsSwitcher.h"

#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"

namespace
{
	void AddSwitchRule(
		TArray<FKeybindingsSwitchRule>& InOutRules,
		E_UIType InUIType,
		E_KeybindingsInputType InInputType,
		const TCHAR* InWidgetName)
	{
		FKeybindingsSwitchRule& Rule = InOutRules.AddDefaulted_GetRef();
		Rule.UIType = InUIType;
		Rule.InputType = InInputType;
		Rule.WidgetName = FName(InWidgetName);
	}
}

UUI_KeybindingsSwitcher::UUI_KeybindingsSwitcher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BuildDefaultSwitchRules();
}

void UUI_KeybindingsSwitcher::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshCurrentSwitch();
}

bool UUI_KeybindingsSwitcher::SwitchByUIType(E_UIType InUIType, E_KeybindingsInputType InInputType)
{
	CurrentUIType = InUIType;
	CurrentInputType = InInputType;

	const FKeybindingsSwitchRule* Rule = FindSwitchRule(InUIType, InInputType);
	if (!Rule || Rule->WidgetName.IsNone())
	{
		return false;
	}

	return SwitchByWidgetName(Rule->WidgetName);
}

bool UUI_KeybindingsSwitcher::SwitchByWidgetName(FName InWidgetName)
{
	if (InWidgetName.IsNone())
	{
		return false;
	}

	const int32 WidgetIndex = FindWidgetIndexByName(InWidgetName);
	if (WidgetIndex == INDEX_NONE)
	{
		return false;
	}

	CurrentWidgetName = InWidgetName;
	return SwitchByWidgetIndex(WidgetIndex);
}

bool UUI_KeybindingsSwitcher::SwitchByWidgetIndex(int32 InWidgetIndex)
{
	if (!WidgetSwitcher_0)
	{
		return false;
	}

	if (InWidgetIndex < 0 || InWidgetIndex >= WidgetSwitcher_0->GetNumWidgets())
	{
		return false;
	}

	WidgetSwitcher_0->SetActiveWidgetIndex(InWidgetIndex);

	if (UWidget* ActiveWidget = WidgetSwitcher_0->GetWidgetAtIndex(InWidgetIndex))
	{
		CurrentWidgetName = FName(*ActiveWidget->GetName());
	}

	return true;
}

void UUI_KeybindingsSwitcher::SetCurrentInputType(E_KeybindingsInputType InInputType)
{
	CurrentInputType = InInputType;
}

bool UUI_KeybindingsSwitcher::RefreshCurrentSwitch()
{
	if (!CurrentWidgetName.IsNone())
	{
		return SwitchByWidgetName(CurrentWidgetName);
	}

	if (CurrentUIType != E_UIType::None)
	{
		return SwitchByUIType(CurrentUIType, CurrentInputType);
	}

	return false;
}

const FKeybindingsSwitchRule* UUI_KeybindingsSwitcher::FindSwitchRule(E_UIType InUIType, E_KeybindingsInputType InInputType) const
{
	const FKeybindingsSwitchRule* ExactRule = SwitchRules.FindByPredicate(
		[InUIType, InInputType](const FKeybindingsSwitchRule& Rule)
		{
			return Rule.UIType == InUIType && Rule.InputType == InInputType;
		});

	if (ExactRule)
	{
		return ExactRule;
	}

	const FKeybindingsSwitchRule* KeyboardFallbackRule = SwitchRules.FindByPredicate(
		[InUIType](const FKeybindingsSwitchRule& Rule)
		{
			return Rule.UIType == InUIType && Rule.InputType == E_KeybindingsInputType::Keyboard;
		});

	if (KeyboardFallbackRule)
	{
		return KeyboardFallbackRule;
	}

	return SwitchRules.FindByPredicate(
		[InUIType](const FKeybindingsSwitchRule& Rule)
		{
			return Rule.UIType == InUIType;
		});
}

int32 UUI_KeybindingsSwitcher::FindWidgetIndexByName(FName InWidgetName) const
{
	if (!WidgetSwitcher_0 || InWidgetName.IsNone())
	{
		return INDEX_NONE;
	}

	for (int32 WidgetIndex = 0; WidgetIndex < WidgetSwitcher_0->GetNumWidgets(); ++WidgetIndex)
	{
		const UWidget* ChildWidget = WidgetSwitcher_0->GetWidgetAtIndex(WidgetIndex);
		if (ChildWidget && ChildWidget->GetFName() == InWidgetName)
		{
			return WidgetIndex;
		}
	}

	return INDEX_NONE;
}

void UUI_KeybindingsSwitcher::BuildDefaultSwitchRules()
{
	if (SwitchRules.Num() > 0)
	{
		return;
	}

	AddSwitchRule(SwitchRules, E_UIType::Character, E_KeybindingsInputType::Keyboard, TEXT("Box_InventoryKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Character, E_KeybindingsInputType::Gamepad, TEXT("Box_InventoryGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Joural, E_KeybindingsInputType::Keyboard, TEXT("Box_QuestKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Joural, E_KeybindingsInputType::Gamepad, TEXT("Box_QuestGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Ability, E_KeybindingsInputType::Keyboard, TEXT("Box_AbilitiesKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Ability, E_KeybindingsInputType::Gamepad, TEXT("Box_AbilitiesGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Crafting, E_KeybindingsInputType::Keyboard, TEXT("Box_CraftingKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Crafting, E_KeybindingsInputType::Gamepad, TEXT("Box_CraftingGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Runes, E_KeybindingsInputType::Keyboard, TEXT("Box_RunesMenuKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::CodeX, E_KeybindingsInputType::Keyboard, TEXT("Box_CodexKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::CodeX, E_KeybindingsInputType::Gamepad, TEXT("Box_CodexGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Glossary, E_KeybindingsInputType::Keyboard, TEXT("Box_CodexKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Glossary, E_KeybindingsInputType::Gamepad, TEXT("Box_CodexGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::Gallery, E_KeybindingsInputType::Keyboard, TEXT("Box_GalleryKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Control, E_KeybindingsInputType::Keyboard, TEXT("Box_ControllerKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Buy, E_KeybindingsInputType::Keyboard, TEXT("Box_VendorKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::Buy, E_KeybindingsInputType::Gamepad, TEXT("Box_VendorGamepad"));
	AddSwitchRule(SwitchRules, E_UIType::SellBack, E_KeybindingsInputType::Keyboard, TEXT("Box_VendorKeyboard"));
	AddSwitchRule(SwitchRules, E_UIType::SellBack, E_KeybindingsInputType::Gamepad, TEXT("Box_VendorGamepad"));
}
