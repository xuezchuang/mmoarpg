#include "UI_InventoryOverall.h"

#include "UI_InventoryBase.h"

void UUI_InventoryOverall::RefreshInventoryWidgets()
{
	if (WB_InventoryBase)
	{
		WB_InventoryBase->RefreshCurrentCategory();
	}
}
