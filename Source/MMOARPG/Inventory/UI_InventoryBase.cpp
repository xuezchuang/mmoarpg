// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_InventoryBase.h"
#include "UI_InventorySlot.h"
#include "UI_CategoryButton.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "GameFramework/PlayerState.h"

void UUI_InventoryBase::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind category button clicks to switch corresponding grids.
	if (WB_CategoryWeapon     && WB_CategoryWeapon->Button_Weapons)     WB_CategoryWeapon->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryWeapon);
	if (WB_CategoryRange      && WB_CategoryRange->Button_Weapons)      WB_CategoryRange->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryRange);
	if (WB_CategoryArmor      && WB_CategoryArmor->Button_Weapons)      WB_CategoryArmor->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryArmor);
	if (WB_CategoryAccessories && WB_CategoryAccessories->Button_Weapons) WB_CategoryAccessories->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryAccessories);
	if (WB_CategoryRuneStone  && WB_CategoryRuneStone->Button_Weapons)  WB_CategoryRuneStone->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryRuneStone);
	if (WB_CategoryConsumable && WB_CategoryConsumable->Button_Weapons) WB_CategoryConsumable->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryConsumable);
	if (WB_CategoryResources  && WB_CategoryResources->Button_Weapons)  WB_CategoryResources->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryResources);
	if (WB_CategoryMount      && WB_CategoryMount->Button_Weapons)      WB_CategoryMount->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryMount);
	if (WB_CategoryQuest      && WB_CategoryQuest->Button_Weapons)      WB_CategoryQuest->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryQuest);
	if (WB_CategoryEvent      && WB_CategoryEvent->Button_Weapons)      WB_CategoryEvent->Button_Weapons->OnClicked.AddDynamic(this, &ThisClass::OnCategoryEvent);

	// Default to weapon category on open.
	SetActiveCategoryButton(WB_CategoryWeapon);
	PopulateGridForCategory(E_InventoryCategory::Weapon);
}

void UUI_InventoryBase::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_InventoryBase::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UUI_InventoryBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bRebuildingFromAutoColumns)
	{
		return;
	}

	UUniformGridPanel* Grid = GetGridByCategory(CurrentCategory);
	if (!Grid)
	{
		return;
	}

	const int32 NewColumns = ResolveRuntimeColumns(Grid);
	if (NewColumns > 0 && NewColumns != RuntimeColumns)
	{
		RuntimeColumns = NewColumns;
		UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] AutoColumns update -> %d, repopulate Category=%d"),
			*GetName(), RuntimeColumns, (int32)CurrentCategory);

		bRebuildingFromAutoColumns = true;
		PopulateGridForCategory(CurrentCategory);
		bRebuildingFromAutoColumns = false;
	}
}

UUniformGridPanel* UUI_InventoryBase::GetGridByCategory(E_InventoryCategory Category) const
{
	switch (Category)
	{
	case E_InventoryCategory::Weapon:      return UniformGrid_P1;
	case E_InventoryCategory::Range:       return UniformGrid_P2;
	case E_InventoryCategory::Armor:       return UniformGrid_P3;
	case E_InventoryCategory::Accessories: return UniformGrid_P4;
	case E_InventoryCategory::RuneStone:   return UniformGrid_P5;
	case E_InventoryCategory::Consumable:  return UniformGrid_P6;
	case E_InventoryCategory::Resources:   return UniformGrid_P7;
	case E_InventoryCategory::Mount:       return UniformGrid_P8;
	case E_InventoryCategory::Quest:       return UniformGrid_P9;
	case E_InventoryCategory::Event:       return UniformGrid_P10;
	default:                               return nullptr;
	}
}

static int32 CategoryToSwitcherIndex(E_InventoryCategory Category)
{
	// Panels_Switcher child order: InventoryPanel1~10
	switch (Category)
	{
	case E_InventoryCategory::Weapon:      return 0;
	case E_InventoryCategory::Range:       return 1;
	case E_InventoryCategory::Armor:       return 2;
	case E_InventoryCategory::Accessories: return 3;
	case E_InventoryCategory::RuneStone:   return 4;
	case E_InventoryCategory::Consumable:  return 5;
	case E_InventoryCategory::Resources:   return 6;
	case E_InventoryCategory::Mount:       return 7;
	case E_InventoryCategory::Quest:       return 8;
	case E_InventoryCategory::Event:       return 9;
	default:                               return 0;
	}
}

int32 UUI_InventoryBase::ResolveRuntimeColumns(UUniformGridPanel* Grid) const
{
	if (!Grid)
	{
		return 0;
	}

	// Prefer the grid's runtime width; fallback to parent width before giving up.
	float AvailableWidth = Grid->GetCachedGeometry().GetLocalSize().X;
	if (AvailableWidth <= 1.f && Grid->GetParent())
	{
		AvailableWidth = Grid->GetParent()->GetCachedGeometry().GetLocalSize().X;
	}
	if (AvailableWidth <= 1.f)
	{
		return 0;
	}

	const float CellPaddingX = 8.f; // SetSlotPadding(FMargin(4.f))
	const float CellWidth = FMath::Max(1.f, SlotMinWidth + CellPaddingX);
	const int32 RawColumns = FMath::Max(1, FMath::FloorToInt((AvailableWidth + CellPaddingX) / CellWidth));
	if (MaxAutoColumns > 0)
	{
		return FMath::Clamp(RawColumns, 1, MaxAutoColumns);
	}
	return RawColumns;
}

void UUI_InventoryBase::PopulateGridForCategory(E_InventoryCategory Category)
{
	CurrentCategory = Category;
	// Switch visible panel.
	if (Panels_Switcher)
	{
		Panels_Switcher->SetActiveWidgetIndex(CategoryToSwitcherIndex(Category));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory:%s] Panels_Switcher is NULL"), *GetName());
	}

	UUniformGridPanel* Grid = GetGridByCategory(Category);
	if (!Grid)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory:%s] Grid is NULL for Category=%d"), *GetName(), (int32)Category);
		return;
	}
	if (!InventorySlotClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory:%s] InventorySlotClass is NULL"), *GetName());
		return;
	}
	const FString ThisPath = GetPathNameSafe(this);
	const FString GridPath = GetPathNameSafe(Grid);
	const FString GridParentPath = GetPathNameSafe(Grid->GetParent());
	UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Instance=%p InViewport=%d Visibility=%s This=%s Grid=%s Parent=%s"),
		*GetName(),
		this,
		IsInViewport() ? 1 : 0,
		*UEnum::GetValueAsString(GetVisibility()),
		*ThisPath,
		*GridPath,
		*GridParentPath);

	if (Panels_Switcher)
	{
		UWidget* Active = Panels_Switcher->GetActiveWidget();
		UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Switcher ActiveIndex=%d ActiveWidget=%s"),
			*GetName(),
			Panels_Switcher->GetActiveWidgetIndex(),
			*GetPathNameSafe(Active));
	}

	if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(Grid->Slot))
	{
		ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
		ScrollSlot->SetVerticalAlignment(VAlign_Fill);

		UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Grid ScrollSlot HAlign=%d VAlign=%d"),
			*GetName(),
			(int32)ScrollSlot->GetHorizontalAlignment(),
			(int32)ScrollSlot->GetVerticalAlignment());
	}

	Grid->ClearChildren();
	Grid->SetSlotPadding(FMargin(4.f));
	Grid->SetMinDesiredSlotWidth(SlotMinWidth);
	Grid->SetMinDesiredSlotHeight(SlotMinHeight);

	// Debug: current runtime geometry for the active grid.
	FVector2D GridLocalSize = Grid->GetCachedGeometry().GetLocalSize();
	UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Grid CachedSize=(%.0f,%.0f)  MinSlot=(%.0f,%.0f)"),
		*GetName(), GridLocalSize.X, GridLocalSize.Y, SlotMinWidth, SlotMinHeight);

	const int32 ResolvedColumns = ResolveRuntimeColumns(Grid);
	if (ResolvedColumns > 0)
	{
		RuntimeColumns = ResolvedColumns;
	}
	const int32 FallbackColumns = FMath::Max(1, Columns);
	const int32 FinalColumns = (RuntimeColumns > 0) ? RuntimeColumns : FallbackColumns;
	UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Config Category=%d  TotalSlots=%d  AutoColumns=%d  LegacyColumns=%d  MaxAuto=%d  ExpectedRows=%d"),
		*GetName(), (int32)Category, TotalSlots, FinalColumns, Columns, MaxAutoColumns, FMath::DivideAndRoundUp(TotalSlots, FinalColumns));

	// Collect items for current category.
	TArray<FFS_ItemData*> Items;
	APlayerController* OwningPC = GetOwningPlayer();
	APlayerController* PC = OwningPC;
	if (!PC)
	{
		if (UWorld* World = GetWorld())
		{
			PC = World->GetFirstPlayerController();
		}
	}

	AMMOARPGPlayerState* PS = PC ? PC->GetPlayerState<AMMOARPGPlayerState>() : nullptr;
	UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] PlayerResolve OwningPC=%s ResolvedPC=%s PlayerState=%s"),
		*GetName(),
		*GetPathNameSafe(OwningPC),
		*GetPathNameSafe(PC),
		*GetPathNameSafe(PS));

	if (PS)
	{
		Items = PS->GetBagItemsByCategory(Category);
		UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Category=%d  BagTotal=%d  Filtered=%d"),
			*GetName(), (int32)Category, PS->GetBagItemCount(), Items.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory:%s] Category=%d cannot resolve AMMOARPGPlayerState"),
			*GetName(), (int32)Category);
	}

	if (Items.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory:%s] Category=%d has no item data"),
			*GetName(), (int32)Category);
	}

	int32 CreatedWidgets = 0;
	int32 AddedToGrid = 0;
	int32 AddFailed = 0;
	int32 LastRow = INDEX_NONE;
	int32 LastCol = INDEX_NONE;
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		UUI_InventorySlot* SlotWidget = CreateWidget<UUI_InventorySlot>(this, InventorySlotClass);
		if (!SlotWidget) { continue; }
		++CreatedWidgets;

		if (Items.IsValidIndex(i))
		{
			SlotWidget->SetItemData(Items[i]);
		}
		else// if (bClearEmptySlotsVisual)
		{
			SlotWidget->SetItemData(nullptr);
		}
		// Keep blueprint default visuals for empty cells unless explicit clear is enabled.
		const int32 Row = i / FinalColumns;
		const int32 Col = i % FinalColumns;
		LastRow = Row;
		LastCol = Col;
		UUniformGridSlot* GSlot = Grid->AddChildToUniformGrid(SlotWidget, Row, Col);
		if (GSlot)
		{
			GSlot->SetHorizontalAlignment(HAlign_Fill);
			GSlot->SetVerticalAlignment(VAlign_Fill);
			++AddedToGrid;
		}
		else
		{
			++AddFailed;
			UE_LOG(LogTemp, Warning, TEXT("[Inventory:%s] AddChildToUniformGrid failed i=%d Row=%d Col=%d"),
				*GetName(), i, Row, Col);
		}

		// Print desired size for the first slot as a quick layout sanity check.
		if (i == 0)
		{
			FVector2D DS = SlotWidget->GetDesiredSize();
			UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] Slot[0] DesiredSize=(%.0f,%.0f)"), *GetName(), DS.X, DS.Y);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] CreateDone Widgets=%d  Added=%d  Failed=%d  GridChildren=%d  LastCell=(%d,%d)"),
		*GetName(), CreatedWidgets, AddedToGrid, AddFailed, Grid->GetChildrenCount(), LastRow, LastCol);

	// Log one more time next tick after layout settles.
	if (UWorld* World = GetWorld())
	{
		TWeakObjectPtr<UUI_InventoryBase> WeakThis(this);
		TWeakObjectPtr<UUniformGridPanel> WeakGrid(Grid);
		const E_InventoryCategory Cat = Category;
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis, WeakGrid, Cat]()
		{
			if (!WeakThis.IsValid() || !WeakGrid.IsValid())
			{
				return;
			}

			const FVector2D PostSize = WeakGrid->GetCachedGeometry().GetLocalSize();
			UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] PostLayout Category=%d  Grid CachedSize=(%.0f,%.0f)  Children=%d"),
				*WeakThis->GetName(), (int32)Cat, PostSize.X, PostSize.Y, WeakGrid->GetChildrenCount());
		}));

		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis, WeakGrid, Cat]()
		{
			if (!WeakThis.IsValid() || !WeakGrid.IsValid())
			{
				return;
			}
			const FVector2D DS = WeakGrid->GetDesiredSize();
			UE_LOG(LogTemp, Log, TEXT("[Inventory:%s] PostLayoutDesired Category=%d  Grid DesiredSize=(%.0f,%.0f)"),
				*WeakThis->GetName(), (int32)Cat, DS.X, DS.Y);
		}));
	}
}

void UUI_InventoryBase::RefreshCurrentCategory()
{
	PopulateGridForCategory(CurrentCategory);
}

// ---- Category button callbacks ----
void UUI_InventoryBase::OnCategoryWeapon()     { SetActiveCategoryButton(WB_CategoryWeapon);      PopulateGridForCategory(E_InventoryCategory::Weapon); }
void UUI_InventoryBase::OnCategoryRange()      { SetActiveCategoryButton(WB_CategoryRange);       PopulateGridForCategory(E_InventoryCategory::Range); }
void UUI_InventoryBase::OnCategoryArmor()      { SetActiveCategoryButton(WB_CategoryArmor);       PopulateGridForCategory(E_InventoryCategory::Armor); }
void UUI_InventoryBase::OnCategoryAccessories(){ SetActiveCategoryButton(WB_CategoryAccessories); PopulateGridForCategory(E_InventoryCategory::Accessories); }
void UUI_InventoryBase::OnCategoryRuneStone()  { SetActiveCategoryButton(WB_CategoryRuneStone);   PopulateGridForCategory(E_InventoryCategory::RuneStone); }
void UUI_InventoryBase::OnCategoryConsumable() { SetActiveCategoryButton(WB_CategoryConsumable);  PopulateGridForCategory(E_InventoryCategory::Consumable); }
void UUI_InventoryBase::OnCategoryResources()  { SetActiveCategoryButton(WB_CategoryResources);   PopulateGridForCategory(E_InventoryCategory::Resources); }
void UUI_InventoryBase::OnCategoryMount()      { SetActiveCategoryButton(WB_CategoryMount);       PopulateGridForCategory(E_InventoryCategory::Mount); }
void UUI_InventoryBase::OnCategoryQuest()      { SetActiveCategoryButton(WB_CategoryQuest);       PopulateGridForCategory(E_InventoryCategory::Quest); }
void UUI_InventoryBase::OnCategoryEvent()      { SetActiveCategoryButton(WB_CategoryEvent);       PopulateGridForCategory(E_InventoryCategory::Event); }

void UUI_InventoryBase::SetActiveCategoryButton(UUI_CategoryButton* ActiveBtn)
{
	UUI_CategoryButton* AllButtons[] = {
		WB_CategoryWeapon, WB_CategoryRange, WB_CategoryArmor,
		WB_CategoryAccessories, WB_CategoryRuneStone, WB_CategoryConsumable,
		WB_CategoryResources, WB_CategoryMount, WB_CategoryQuest, WB_CategoryEvent
	};
	for (UUI_CategoryButton* Btn : AllButtons)
	{
		if (Btn) Btn->SetActive(Btn == ActiveBtn);
	}
}
