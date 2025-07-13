// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGHUD.h"
#include "../../UI/Game/UI_GameMain.h"

AMMOARPGHUD::AMMOARPGHUD()
{
	static ConstructorHelpers::FClassFinder<UUI_GameMain> GameMain_BPClass(TEXT("/Game/UI/Game/UI_GameMain_BP"));
	GameMainClass = GameMain_BPClass.Class;
}

void AMMOARPGHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GameMainClass)
	{
		GameMain = CreateWidget<UUI_GameMain>(GetWorld(), GameMainClass);
		GameMain->AddToViewport();
	}
}

