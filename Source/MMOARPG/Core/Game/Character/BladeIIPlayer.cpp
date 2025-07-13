// Copyright Epic Games, Inc. All Rights Reserved.

#include "BladeIIPlayer.h"
#include "ThreadManage.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Core/Component/FlyComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MMOARPGMacroType.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPG.h"
//////////////////////////////////////////////////////////////////////////
// ABladeIIPlayer

ABladeIIPlayer::ABladeIIPlayer()
{
	
}

void ABladeIIPlayer::BeginPlay()
{
	Super::BeginPlay();
}

