#include "UI_InventoryOverall.h"

#include "UI_InventoryBase.h"
#include "UI_GoldWeightBar.h"

void UUI_InventoryOverall::RefreshInventoryWidgets()
{
	if (WB_GoldWeightBar)
	{
		WB_GoldWeightBar->RefreshDisplay();
	}

	if (WB_InventoryBase)
	{
		WB_InventoryBase->RefreshCurrentCategory();
	}
}
