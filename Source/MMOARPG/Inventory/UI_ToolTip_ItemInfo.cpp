#include "UI_ToolTip_ItemInfo.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/Widget.h"
#include "../Core/Game/MMOARPGGameState.h"
#include "UI_ConsumableTypeIcon.h"

namespace
{
	FString BeautifyEnumName(FString InName)
	{
		InName.ReplaceInline(TEXT("_"), TEXT(" "));
		return InName;
	}

	FText GetItemClassText(const FFS_ItemData& ItemData)
	{
		if (ItemData.WeaponCategory != E_WeaponCategory::None)
		{
			return FText::FromString(BeautifyEnumName(StaticEnum<E_WeaponCategory>()->GetNameStringByValue(static_cast<int64>(ItemData.WeaponCategory))));
		}

		return FText::FromString(BeautifyEnumName(StaticEnum<E_ItemType>()->GetNameStringByValue(static_cast<int64>(ItemData.Type))));
	}

	FText GetStatNameText(E_StatType StatType)
	{
		return FText::FromString(BeautifyEnumName(StaticEnum<E_StatType>()->GetNameStringByValue(static_cast<int64>(StatType))));
	}

	FText GetConsumableStatNameText(E_StatCategory StatCategory)
	{
		return FText::FromString(BeautifyEnumName(StaticEnum<E_StatCategory>()->GetNameStringByValue(static_cast<int64>(StatCategory))));
	}

	bool IsPercentStat(E_StatType StatType)
	{
		switch (StatType)
		{
		case E_StatType::CriticalRate:
		case E_StatType::CriticalDamage:
		case E_StatType::PhAttackSpeed:
		case E_StatType::Block:
		case E_StatType::AStaminaCost:
		case E_StatType::CastingSpeed:
		case E_StatType::MountMaxSpeed:
		case E_StatType::MountMaxStamina:
		case E_StatType::MountAcceleration:
		case E_StatType::MountDeceleration:
		case E_StatType::MountKickStaminaCost:
		case E_StatType::RnAttackSpeed:
		case E_StatType::EffectDuration:
			return true;
		default:
			return false;
		}
	}

	FText GetStatValueText(E_StatType StatType, float Value)
	{
		if (IsPercentStat(StatType))
		{
			return FText::FromString(FString::Printf(TEXT("%.0f%%"), Value));
		}

		if (FMath::IsNearlyEqual(Value, FMath::RoundToFloat(Value)))
		{
			return FText::AsNumber(FMath::RoundToInt(Value));
		}

		return FText::FromString(FString::Printf(TEXT("%.1f"), Value));
	}

	FText GetConsumableValueText(float Value)
	{
		if (FMath::IsNearlyEqual(Value, FMath::RoundToFloat(Value)))
		{
			return FText::AsNumber(FMath::RoundToInt(Value));
		}

		return FText::FromString(FString::Printf(TEXT("%.1f"), Value));
	}

	void SetOptionalText(UTextBlock* LabelWidget, UTextBlock* ValueWidget, const FText& LabelText, const FText& ValueText, bool bVisible)
	{
		if (LabelWidget)
		{
			LabelWidget->SetText(bVisible ? LabelText : FText::GetEmpty());
			LabelWidget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}

		if (ValueWidget)
		{
			ValueWidget->SetText(bVisible ? ValueText : FText::GetEmpty());
			ValueWidget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}

	int32 GetRarityStarCount(E_ItemRarity Rarity)
	{
		switch (Rarity)
		{
		case E_ItemRarity::Common: return 1;
		case E_ItemRarity::Superior: return 2;
		case E_ItemRarity::Epic: return 3;
		case E_ItemRarity::Legendary: return 4;
		default: return 0;
		}
	}

	FLinearColor GetFallbackRarityColor(E_ItemRarity Rarity)
	{
		switch (Rarity)
		{
		case E_ItemRarity::Superior:   return FLinearColor(0.18f, 0.62f, 0.96f, 1.0f);
		case E_ItemRarity::Epic:       return FLinearColor(0.66f, 0.33f, 0.90f, 1.0f);
		case E_ItemRarity::Legendary:  return FLinearColor(1.00f, 0.62f, 0.14f, 1.0f);
		case E_ItemRarity::Consumable: return FLinearColor(0.34f, 0.82f, 0.39f, 1.0f);
		case E_ItemRarity::Usable:     return FLinearColor(0.93f, 0.80f, 0.24f, 1.0f);
		case E_ItemRarity::Common:
		default:                       return FLinearColor(0.74f, 0.74f, 0.74f, 1.0f);
		}
	}

	FLinearColor GetTooltipRarityColor(const UWorld* World, E_ItemRarity Rarity)
	{
		if (World)
		{
			if (AMMOARPGGameState* GameState = World->GetGameState<AMMOARPGGameState>())
			{
				return GameState->GetRarityColor(Rarity).GetSpecifiedColor();
			}
		}

		return GetFallbackRarityColor(Rarity);
	}

	void ApplyTint(UImage* Image, const FLinearColor& BaseColor, float Alpha)
	{
		if (!Image)
		{
			return;
		}

		FLinearColor Tint = BaseColor;
		Tint.A = Alpha;
		Image->SetColorAndOpacity(Tint);
	}
}

void UUI_ToolTip_ItemInfo::SetItemData(const FFS_ItemData* InItemData)
{
	if (!InItemData)
	{
		if (ItemName)
		{
			ItemName->SetText(FText::GetEmpty());
		}
		ApplyTint(BG, FLinearColor::White, 1.0f);
		ApplyTint(HeaderBG, FLinearColor::White, 1.0f);
		if (ItemClassText)
		{
			ItemClassText->SetText(FText::GetEmpty());
		}
		SetOptionalText(N_MainStat, T_MainStat, FText::GetEmpty(), FText::GetEmpty(), false);
		SetOptionalText(N_SecondaryStat, T_SecondaryStat, FText::GetEmpty(), FText::GetEmpty(), false);
		if (ItemIcon)
		{
			ItemIcon->SetBrushResourceObject(nullptr);
		}
		if (WB_Icon_Consumable_Type)
		{
			WB_Icon_Consumable_Type->SetByItemData(nullptr);
		}
		if (Equipped)
		{
			Equipped->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (CompareText)
		{
			CompareText->SetVisibility(ESlateVisibility::Collapsed);
			CompareText->SetText(FText::GetEmpty());
		}
		if (CloneGridBoost)
		{
			for (int32 ChildIndex = 0; ChildIndex < CloneGridBoost->GetChildrenCount(); ++ChildIndex)
			{
				if (UWidget* Child = CloneGridBoost->GetChildAt(ChildIndex))
				{
					Child->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
		return;
	}

	if (ItemName)
	{
		ItemName->SetText(InItemData->Description.Name);
	}

	const FLinearColor RarityColor = GetTooltipRarityColor(GetWorld(), InItemData->Rarity);
	ApplyTint(BG, RarityColor, 0.35f);
	ApplyTint(HeaderBG, RarityColor, 0.85f);

	if (ItemClassText)
	{
		ItemClassText->SetText(GetItemClassText(*InItemData));
	}

	if (ItemIcon)
	{
		ItemIcon->SetBrushResourceObject(InItemData->Image);
	}

	if (WB_Icon_Consumable_Type)
	{
		WB_Icon_Consumable_Type->SetByItemData(InItemData);
	}

	if (Equipped)
	{
		Equipped->SetVisibility(InItemData->IsEquipped ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (CompareText)
	{
		CompareText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InItemData->Stats.Stats.Num() > 0)
	{
		const FF_Modifier& MainStat = InItemData->Stats.Stats[0];
		SetOptionalText(N_MainStat, T_MainStat, GetStatNameText(MainStat.Type), GetStatValueText(MainStat.Type, MainStat.Value), true);

		if (InItemData->Stats.Stats.Num() > 1)
		{
			const FF_Modifier& SecondaryStat = InItemData->Stats.Stats[1];
			SetOptionalText(N_SecondaryStat, T_SecondaryStat, GetStatNameText(SecondaryStat.Type), GetStatValueText(SecondaryStat.Type, SecondaryStat.Value), true);
		}
		else
		{
			SetOptionalText(N_SecondaryStat, T_SecondaryStat, FText::GetEmpty(), FText::GetEmpty(), false);
		}
	}
	else if ((InItemData->Type == E_ItemType::Food) || (InItemData->Type == E_ItemType::Potion))
	{
		SetOptionalText(
			N_MainStat,
			T_MainStat,
			GetConsumableStatNameText(InItemData->Stats.ConsumableAction.Stat),
			GetConsumableValueText(InItemData->Stats.ConsumableAction.Value),
			true);
		SetOptionalText(N_SecondaryStat, T_SecondaryStat, FText::GetEmpty(), FText::GetEmpty(), false);
	}
	else
	{
		SetOptionalText(N_MainStat, T_MainStat, FText::GetEmpty(), FText::GetEmpty(), false);
		SetOptionalText(N_SecondaryStat, T_SecondaryStat, FText::GetEmpty(), FText::GetEmpty(), false);
	}

	if (CloneGridBoost)
	{
		const int32 StarCount = GetRarityStarCount(InItemData->Rarity);
		for (int32 ChildIndex = 0; ChildIndex < CloneGridBoost->GetChildrenCount(); ++ChildIndex)
		{
			if (UWidget* Child = CloneGridBoost->GetChildAt(ChildIndex))
			{
				Child->SetVisibility(ChildIndex < StarCount ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			}
		}
	}
}
