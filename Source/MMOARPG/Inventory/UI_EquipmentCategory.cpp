#include "UI_EquipmentCategory.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUI_EquipmentCategory::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_82)
	{
		Button_82->OnClicked.__Internal_AddDynamic(this, &ThisClass::HandleButtonClicked, TEXT("HandleButtonClicked"));
	}

	SetSelected(bSelected);
	SetCategoryText(CategoryLabel);
}

void UUI_EquipmentCategory::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_EquipmentCategory::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetCategoryText(CategoryLabel);
}

void UUI_EquipmentCategory::SetCategoryText(const FText& InText)
{
	CategoryLabel = InText;

	if (CategoryText)
	{
		CategoryText->SetText(CategoryLabel);
	}
}

void UUI_EquipmentCategory::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	BP_OnSelectedChanged(bSelected);
}

void UUI_EquipmentCategory::HandleButtonClicked()
{
	OnCategoryClicked.Broadcast(this);
}
