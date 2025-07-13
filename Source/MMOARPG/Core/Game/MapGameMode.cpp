// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapGameMode.h"
//#include "Character/MMOARPGCharacter.h"
//#include "UObject/ConstructorHelpers.h"
//#include "MMOARPGHUD.h"
//#include "ThreadManage.h"
//#include "UObject/SimpleController.h"
//#include "MMOARPGMacroType.h"
//#include "MMOARPGPlayerState.h"
//#include "MMOARPGGameState.h"
//#include "ThreadManage.h"
//#include "Character/MMOARPGPlayerCharacter.h"
//#include "Protocol/GameProtocol.h"
//#include "Core/MethodUnit.h"
//#include "MMOARPGPlayerController.h"

AMapGameMode::AMapGameMode()
{
}

void AMapGameMode::BeginPlay()
{
	Super::BeginPlay();

	//CreateWidget<UUI_EditMap>(GetWorld(), UIEditMap_BPClass)->AddToViewport();
}

void AMapGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

