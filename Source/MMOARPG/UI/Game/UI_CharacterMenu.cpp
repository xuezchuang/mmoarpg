#include "UI_CharacterMenu.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetTree.h"
#include "Core/Game/MMOARPGPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Input/Reply.h"
#include "Inventory/UI_InventoryBase.h"
#include "Inventory/UI_InventoryOverall.h"
#include "Inventory/UI_LevelExp.h"
#include "Inventory/UI_LoadingScreen.h"
#include "Inventory/UI_WindowSwitcher.h"
#include "TimerManager.h"

UUI_CharacterMenu::UUI_CharacterMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UUI_CharacterMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (WB_WindowSwitcher)
	{
		WB_WindowSwitcher->InitOutIcon(HeaderIcon, ActiveTextTab, E_UpSwitchType::MainTab);
		WB_WindowSwitcher->WindowSwitchDelegate.BindUObject(this, &UUI_CharacterMenu::HandleTabChanged);
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this, &UUI_CharacterMenu::HandleCloseButtonClicked);
	}

	if (WB_LoadingScreen)
	{
		WB_LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetTopInfoBarShowLevelExp(WB_LevelExp != nullptr);

	Super::SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_CharacterMenu::NativeDestruct()
{
	CloseMenu();

	Super::NativeDestruct();
}

FReply UUI_CharacterMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey InputKey = InKeyEvent.GetKey();
	if (InputKey == EKeys::R || InputKey == EKeys::I || InputKey == EKeys::Escape)
	{
		CloseMenu();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UUI_CharacterMenu::ApplyMenuConfig(const FCharacterMenuConfig& InConfig)
{
	MenuConfig = InConfig;

	E_UIType DesiredTab = MenuConfig.DefaultTab;
	if (ActiveTab != E_UIType::None && FindTabData(ActiveTab))
	{
		DesiredTab = ActiveTab;
	}

	SetActiveTab(DesiredTab);
}

void UUI_CharacterMenu::OpenMenu()
{
	if (!bIsMenuOpen)
	{
		if (!IsInViewport())
		{
			AddToViewport(ViewportZOrder);
		}

		Super::SetVisibility(ESlateVisibility::Visible);
		bIsMenuOpen = true;
	}

	if (ActiveTab == E_UIType::None)
	{
		SetActiveTab(MenuConfig.DefaultTab);
	}
	else
	{
		SetActiveTab(ActiveTab);
	}

	RefreshInventoryWidgets();
	TriggerLoadingScreen(EUMGSequencePlayMode::Forward);
	ApplyInputMode(true);
	BP_OnMenuOpened();
}

void UUI_CharacterMenu::CloseMenu()
{
	if (!bIsMenuOpen && GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	bIsMenuOpen = false;
	ApplyInputMode(false);
	BP_OnMenuClosed();

	if (WB_LoadingScreen)
	{
		TriggerLoadingScreen(EUMGSequencePlayMode::Reverse);

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CloseMenuTimerHandle);
			const float Delay = FMath::Max(0.f, WB_LoadingScreen->GetLoadingScreenDuration());
			if (Delay > KINDA_SMALL_NUMBER)
			{
				World->GetTimerManager().SetTimer(CloseMenuTimerHandle, this, &UUI_CharacterMenu::FinishCloseMenu, Delay, false);
				return;
			}
		}
	}

	FinishCloseMenu();
}

void UUI_CharacterMenu::ToggleMenu()
{
	if (bIsMenuOpen)
	{
		CloseMenu();
	}
	else
	{
		OpenMenu();
	}
}

void UUI_CharacterMenu::SetActiveTab(E_UIType InTabType)
{
	if (InTabType == E_UIType::None)
	{
		InTabType = MenuConfig.DefaultTab;
	}

	const FCharacterMenuTabData* TabData = FindTabData(InTabType);
	if (!TabData && MenuConfig.Tabs.Num() > 0)
	{
		TabData = &MenuConfig.Tabs[0];
		InTabType = TabData->TabType;
	}

	ActiveTab = InTabType;

	if (WB_WindowSwitcher)
	{
		WB_WindowSwitcher->ApplyType(ActiveTab, false);
	}

	if (ContentSwitcher && TabData && ContentSwitcher->GetNumWidgets() > 0)
	{
		const int32 ClampedIndex = FMath::Clamp(TabData->WidgetIndex, 0, ContentSwitcher->GetNumWidgets() - 1);
		ContentSwitcher->SetActiveWidgetIndex(ClampedIndex);
	}

	BP_OnActiveTabChanged(ActiveTab);
}

static void FindInventoryWidgetsRecursive(UUserWidget* UserWidget, TArray<UUI_InventoryBase*>& OutFound)
{
	if (!UserWidget || !UserWidget->WidgetTree)
	{
		return;
	}

	TArray<UWidget*> Widgets;
	UserWidget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Widget : Widgets)
	{
		if (UUI_InventoryBase* Inv = Cast<UUI_InventoryBase>(Widget))
		{
			OutFound.Add(Inv);
		}
		else if (UUserWidget* Child = Cast<UUserWidget>(Widget))
		{
			// GetAllWidgets 只搜当前 WidgetTree，不进入子 UserWidget 的 WidgetTree。
			// 必须手动递归，否则找不到多层嵌套的 UUI_InventoryBase。
			FindInventoryWidgetsRecursive(Child, OutFound);
		}
	}
}

void UUI_CharacterMenu::RefreshInventoryWidgets()
{
	if (WB_Inventory_Overall)
	{
		WB_Inventory_Overall->RefreshInventoryWidgets();
		return;
	}

	TArray<UUI_InventoryBase*> InventoryWidgets;
	FindInventoryWidgetsRecursive(this, InventoryWidgets);
	for (UUI_InventoryBase* InventoryWidget : InventoryWidgets)
	{
		InventoryWidget->RefreshCurrentCategory();
	}
}

void UUI_CharacterMenu::SetTopInfoBarShowLevelExp(bool bShowLevelExp)
{
	if (WB_LevelExp)
	{
		WB_LevelExp->SetVisibility(bShowLevelExp ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

const FCharacterMenuTabData* UUI_CharacterMenu::FindTabData(E_UIType InTabType) const
{
	return MenuConfig.Tabs.FindByPredicate([InTabType](const FCharacterMenuTabData& InTabData)
	{
		return InTabData.TabType == InTabType;
	});
}

void UUI_CharacterMenu::ApplyInputMode(bool bMenuOpen)
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		PlayerController = GetPlayerController<APlayerController>();
	}

	if (!PlayerController)
	{
		return;
	}

	if (AMMOARPGPlayerController* MMOARPGPC = Cast<AMMOARPGPlayerController>(PlayerController))
	{
		MMOARPGPC->SetMainUserWidgetVisibility(bMenuOpen ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}

	if (bMenuOpen)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
		SetKeyboardFocus();
	}
	else
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}
}

void UUI_CharacterMenu::HandleTabChanged(E_UIType InTabType)
{
	SetActiveTab(InTabType);
	TriggerLoadingScreen(EUMGSequencePlayMode::Forward);
}

void UUI_CharacterMenu::HandleCloseButtonClicked()
{
	CloseMenu();
}

void UUI_CharacterMenu::FinishCloseMenu()
{
	Super::SetVisibility(ESlateVisibility::Collapsed);
	HideLoadingScreen();
}

void UUI_CharacterMenu::HideLoadingScreen()
{
	if (WB_LoadingScreen)
	{
		WB_LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_CharacterMenu::TriggerLoadingScreen(EUMGSequencePlayMode::Type PlayMode)
{
	if (!WB_LoadingScreen)
	{
		return;
	}

	WB_LoadingScreen->PlayLoadingScreenAnim(PlayMode);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LoadingScreenHideTimerHandle);

		const float Delay = FMath::Max(0.f, WB_LoadingScreen->GetLoadingScreenDuration());
		if (Delay > KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().SetTimer(LoadingScreenHideTimerHandle, this, &UUI_CharacterMenu::HideLoadingScreen, Delay, false);
		}
	}
}
