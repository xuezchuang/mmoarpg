// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "SimpleNetChannelType.h"
#include "MMOARPGType.h"
#include "MapGameMode.generated.h"

class FSimpleChannel;
class UUI_EditMap;

UCLASS()
class AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = UI)
	//TSubclassOf<UUI_EditMap> UIEditMap_BPClass;

public:
	AMapGameMode();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);



};



