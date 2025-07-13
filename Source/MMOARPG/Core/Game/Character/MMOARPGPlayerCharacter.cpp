// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGPlayerCharacter.h"
#include "../../Common/MMOARPGGameInstance.h"
#include "../MMOARPGGameMode.h"
//#include "ThreadManage.h"
#include "MMOARPGMacroType.h"
#include "../MMOARPGPlayerState.h"

void AMMOARPGPlayerCharacter::UpdateKneadingBoby(const FMMOARPGCharacterAppearance& InCA)
{
	SetLegSize(InCA.LegSize);
	SetWaistSize(InCA.WaistSize);
	SetArmSize(InCA.ArmSize);

	//需要Mesh也要和我们的骨骼一致变化
	SetMeshPostion(GetMesh());
}
//
//void AMMOARPGPlayerCharacter::CallUpdateKneadingBobyOnClient_Implementation(const FMMOARPGCharacterAppearance& InCA)
//{
//	UpdateKneadingBoby(InCA);
//
//	if (GetLocalRole()== ENetRole::ROLE_AutonomousProxy)
//	{
//		if (AMMOARPGPlayerState* InPlayerState = GetPlayerState<AMMOARPGPlayerState>())
//		{
//			InPlayerState->GetCA() = InCA;
//		}
//	}
//}

void AMMOARPGPlayerCharacter::UpdateKneadingBoby()
{
	
}

void AMMOARPGPlayerCharacter::CallServerUpdateKneading_Implementation(int32 InUserID)
{
	if (AMMOARPGGameMode *InGameMode = GetWorld()->GetAuthGameMode<AMMOARPGGameMode>())
	{
		UserID = InUserID;
		InGameMode->LoginCharacterUpdateKneadingRequest(InUserID);	
	}
}

void AMMOARPGPlayerCharacter::UpdateKneadingRequest()
{
	if (UMMOARPGGameInstance* InGameInstance = GetWorld()->GetGameInstance<UMMOARPGGameInstance>())
	{
#if UE_MMOARPG_DEBUG_DS
		//RPC
		CallServerUpdateKneading(1);
#else
		//RPC
		CallServerUpdateKneading(InGameInstance->GetUserData().ID);
#endif		
	}
}

void AMMOARPGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitKneadingLocation(GetMesh()->GetComponentLocation());

	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		if (AMMOARPGPlayerState* InPlayerState = GetPlayerState<AMMOARPGPlayerState>())
		{
			UpdateKneadingBoby(InPlayerState->GetCA());
		}

#if !UE_MMOARPG_DEBUG_DS
		UpdateKneadingRequest();
#endif
	}
	else if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{

	}
}
