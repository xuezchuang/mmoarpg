// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapGameMode.h"
//#include "UI_EditMap.h"

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

