// Fill out your copyright notice in the Description page of Project Settings.
#include "LoginGameMode.h"
#include "LoginHUD.h"
#include "LoginPawn.h"
#include "LoginPlayerController.h"
#include "MMOARPG.h"

ALoginGameMode::ALoginGameMode()
{
	HUDClass = ALoginHUD::StaticClass();
	DefaultPawnClass = ALoginPawn::StaticClass();
	PlayerControllerClass = ALoginPlayerController::StaticClass();

	UE_LOG(MMOARPG, Display, TEXT("123"));
}

