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
#include "Component/SelectableComponent.h"
//////////////////////////////////////////////////////////////////////////
// ABladeIIPlayer

ABladeIIPlayer::ABladeIIPlayer()
{
	SelectableComp = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComp"));
}

void ABladeIIPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (SelectableComp)
	{
		SelectableComp->OnSelectedEvent.AddDynamic(this, &ABladeIIPlayer::HandleSelected);
		SelectableComp->OnSelectionEndEvent.AddDynamic(this, &ABladeIIPlayer::HandleSelectionEnd);
	}
}

void ABladeIIPlayer::HandleSelected_Implementation()
{

}

void ABladeIIPlayer::HandleSelectionEnd_Implementation()
{

}


