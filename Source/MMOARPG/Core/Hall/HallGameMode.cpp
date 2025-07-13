// Fill out your copyright notice in the Description page of Project Settings.
#include "HallGameMode.h"
#include "HallHUD.h"
#include "HallPawn.h"
#include "HallPlayerController.h"
#include "HallPlayerState.h"
//#include "../Common/MMOARPGGameInstance.h"

AHallGameMode::AHallGameMode()
{
	HUDClass = AHallHUD::StaticClass();
	DefaultPawnClass = AHallPawn::StaticClass();
	PlayerControllerClass = AHallPlayerController::StaticClass();
	PlayerStateClass = AHallPlayerState::StaticClass();
}

void AHallGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//if(UMMOARPGGameInstance* InGameInstance=GetWorld()->GetGameInstance<UMMOARPGGameInstance>())
	//{
	//	InGameInstance->LinkServer(InGameInstance->GetGateStatus().GateServerAddrInfo);
	//}
}

