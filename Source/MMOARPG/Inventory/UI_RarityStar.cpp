#include "UI_RarityStar.h"
#include "Components/Image.h"

void UUI_RarityStar::SetStarTexture(UTexture2D* InTexture)
{
	if (Image_51)
	{
		Image_51->SetBrushResourceObject(InTexture);
	}
}

void UUI_RarityStar::SetStarVisible(bool bVisible)
{
	if (Image_51)
	{
		Image_51->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}
