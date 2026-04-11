#include "UI_ConsumableTypeIcon.h"
#include "Components/Image.h"
#include "../Core/Component/InventoryComponent.h"
#include "../MMOARPG.h"

void UUI_ConsumableTypeIcon::SetByItemData(const FFS_ItemData* InItemData)
{
	const bool bValidConsumable =
		InItemData &&
		((InItemData->Type == E_ItemType::Food) || (InItemData->Type == E_ItemType::Potion));

	if (!bValidConsumable)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		if (ConsumableTypeUsed)
		{
			ConsumableTypeUsed->SetVisibility(ESlateVisibility::Collapsed);
			ConsumableTypeUsed->SetBrushFromTexture(nullptr);
		}
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!ConsumableTypeUsed)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[ConsumableIcon] ConsumableTypeUsed is null [Item:%s Type:%d]"),
			*InItemData->Description.Name.ToString(),
			static_cast<int32>(InItemData->Type));
		return;
	}

	const E_StatCategory Stat = InItemData->Stats.ConsumableAction.Stat;
	UInventoryComponent* InventoryComponent = UInventoryComponent::GetInventoryComponent(this);
	if (!InventoryComponent)
	{
		ConsumableTypeUsed->SetBrushFromTexture(nullptr);
		ConsumableTypeUsed->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(MMOARPG, Warning, TEXT("[ConsumableIcon] InventoryComponent is null [Item:%s Type:%d Stat:%d]"),
			*InItemData->Description.Name.ToString(),
			static_cast<int32>(InItemData->Type),
			static_cast<int32>(Stat));
		return;
	}

	UTexture2D* IconTexture = InventoryComponent->GetStatTexture(Stat);
	if (!IconTexture && Stat != E_StatCategory::None)
	{
		IconTexture = InventoryComponent->GetStatTexture(E_StatCategory::None);
	}

	if (IconTexture)
	{
		ConsumableTypeUsed->SetBrushFromTexture(IconTexture);
		ConsumableTypeUsed->SetVisibility(ESlateVisibility::HitTestInvisible);
		UE_LOG(MMOARPG, Display, TEXT("[ConsumableIcon] Show icon [Item:%s Type:%d Stat:%d]"),
			*InItemData->Description.Name.ToString(),
			static_cast<int32>(InItemData->Type),
			static_cast<int32>(Stat));
	}
	else
	{
		ConsumableTypeUsed->SetBrushFromTexture(nullptr);
		ConsumableTypeUsed->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(MMOARPG, Warning, TEXT("[ConsumableIcon] Icon map miss [Item:%s Type:%d Stat:%d]"),
			*InItemData->Description.Name.ToString(),
			static_cast<int32>(InItemData->Type),
			static_cast<int32>(Stat));
	}
}
