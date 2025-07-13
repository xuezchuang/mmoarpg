#include "MMOARPGBPLibrary.h"
#include "GameFramework\PlayerController.h"
#include "Components\Widget.h"

UMMOARPGBPLibrary::UMMOARPGBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMMOARPGBPLibrary::MMOARPG_InputMode(const UObject* WorldContextObject, EMMOARPG_InputMode Input, UWidget* pWidget /* = NULL */)
{
	if(UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		APlayerController* PlayerController = World->GetFirstPlayerController<APlayerController>();
		if(Input == EMMOARPG_InputMode::Input_UI)
		{
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			if(pWidget)
				InputMode.SetWidgetToFocus(pWidget->TakeWidget());
			PlayerController->SetInputMode(InputMode);
		}
		else if(Input == EMMOARPG_InputMode::Input_Game)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
		}
	}
}

void UMMOARPGBPLibrary::MMOARPG_Test(const UObject* WorldContextObject)
{

}

void UMMOARPGBPLibrary::MMOARPG_Test2()
{

}

