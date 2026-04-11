#include "UI_ToolTip.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "UI_ToolTip_ItemInfo.h"

namespace
{
	FText FormatWeightText(float InValue)
	{
		return FText::FromString(FString::Printf(TEXT("%.1f"), InValue));
	}
}

void UUI_ToolTip::SetItemData(const FFS_ItemData* InItemData)
{
	if (!InItemData)
	{
		SetVisibility(ESlateVisibility::Collapsed);

		if (WB_ToolTip_ItemInfo)
		{
			WB_ToolTip_ItemInfo->SetItemData(nullptr);
		}
		if (Description)
		{
			Description->SetText(FText::GetEmpty());
		}
		if (Weight)
		{
			Weight->SetText(FText::GetEmpty());
		}
		if (price)
		{
			price->SetText(FText::GetEmpty());
		}
		return;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (WB_ToolTip_ItemInfo)
	{
		WB_ToolTip_ItemInfo->SetItemData(InItemData);
	}

	if (Description)
	{
		Description->SetText(InItemData->Description.Description);
	}

	if (ItemDescription)
	{
		const bool bHasDescription = !InItemData->Description.Description.IsEmpty();
		ItemDescription->SetVisibility(bHasDescription ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (Weight)
	{
		Weight->SetText(FormatWeightText(InItemData->Stats.Weight));
	}

	if (price)
	{
		price->SetText(FText::AsNumber(FMath::RoundToInt(InItemData->Stats.Value)));
	}

	if (WeightandCurrency)
	{
		WeightandCurrency->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
