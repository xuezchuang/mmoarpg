#include "UI_LoadingScreen.h"

void UUI_LoadingScreen::PlayLoadingScreenAnim(EUMGSequencePlayMode::Type PlayMode)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayWidgetAnim(TEXT("LoadingScreen"), PlayMode);
}

float UUI_LoadingScreen::GetLoadingScreenDuration() const
{
	if (UWidgetBlueprintGeneratedClass* WidgetBlueprintGenerated = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		for (UWidgetAnimation* Animation : WidgetBlueprintGenerated->Animations)
		{
			if (Animation && Animation->GetFName().ToString() == TEXT("LoadingScreen_INST"))
			{
				return Animation->GetEndTime() - Animation->GetStartTime();
			}
		}
	}

	return 0.f;
}
