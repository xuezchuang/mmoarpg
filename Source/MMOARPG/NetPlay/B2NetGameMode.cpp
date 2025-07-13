// Copyright Epic Games, Inc. All Rights Reserved.

#include "B2NetGameMode.h"
#include "MMOARPG.h"
#include "ThreadManage.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/SimpleController.h"
#include "MMOARPGMacroType.h"
#include "MMOARPGPlayerCharacter.h"
#include "Protocol/GameProtocol.h"
#include "Common/MMOARPGGameInstance.h"
#include "MMOARPGPlayerState.h"
#include "MMOARPGGameState.h"
#include "MMOARPGHUD.h"
#include "MMOARPGPlayerController.h"
#include "BladeIINetPlayer.h"

ABladeIINetGameMode::ABladeIINetGameMode()
{
	HUDClass = AMMOARPGHUD::StaticClass();
	PlayerStateClass = AMMOARPGPlayerState::StaticClass();
	GameStateClass = AMMOARPGGameState::StaticClass();
	PlayerControllerClass = AMMOARPGPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABladeIINetGameMode::BeginPlay()
{
	Super::BeginPlay();

	MapOtherCharacter.Empty();

	UE_LOG(MMOARPG, Display, TEXT("123"));
	LinkServer();
}

void ABladeIINetGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	//{
	//	if (InGameInstance->GetClient() && InGameInstance->GetClient()->GetController())
	//	{
	//		InGameInstance->GetClient()->GetController()->RecvDelegate.Remove(RecvDelegate);
	//	}
	//}
}

// Called every frame
void ABladeIINetGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABladeIINetGameMode::LoginCharacterUpdateKneadingRequest(int32 InUserID)
{
	//SEND_DATA(SP_UpdateLoginCharacterInfoRequests,InUserID);
}

void ABladeIINetGameMode::BindClientRcv()
{
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (InGameInstance->GetClient() && InGameInstance->GetClient()->GetController())
		{
			SEND_DATA(SP_EnterWorld);
			RecvDelegate = InGameInstance->GetClient()->GetController()->RecvDelegate.AddLambda(
				[&](uint32 ProtocolNumber, FSimpleChannel* Channel)
				{
					this->RecvProtocol(ProtocolNumber, Channel);
				});
		}
		else
		{
			GThread::Get()->GetCoroutines().BindLambda(0.5f, [&]() { BindClientRcv(); });
		}
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.5f, [&]() { BindClientRcv(); });
	}
}

void ABladeIINetGameMode::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
{

}

void ABladeIINetGameMode::LinkServer()
{
	//创建客户端
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		InGameInstance->CreateClient();
		if (InGameInstance->GetClient())
		{
			InGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ABladeIINetGameMode::LinkServerInfo);

			//InGameInstance->LinkServer();
			//
			//作为测试
			//InGameInstance->LinkServer(TEXT("127.0.0.1"),11231);

			BindClientRcv();
		}
	}
}

void ABladeIINetGameMode::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch(ProtocolNumber)
	{
	case SP_UpdatePos:
	{
		uint16 childcmd;
		SIMPLE_PROTOCOLS_RECEIVE(SP_EnterWorld, childcmd);
		if(childcmd != 0)
		{
			UE_LOG(MMOARPG, Error, TEXT("Recv SP_UpdatePos [childcmd:%d]"), childcmd);
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("Recv SP_UpdatePos [childcmd:%d]"), childcmd));
		}

		break;
	}
	case SP_UpdateOtherPos:
	{
		S_MOVE_ROLE rMove;
		SIMPLE_PROTOCOLS_RECEIVE(SP_UpdateOtherPos, rMove);
		if(!MapOtherCharacter.Contains(rMove.userindex))
		{
			SIMPLE_PROTOCOLS_SEND(SP_RoleBaseInfo, rMove.userindex);
		}
		else
		{
			MapOtherCharacter[rMove.userindex]->UpdateMoveData(&rMove);
		}
		break;
	}
	case SP_EnterWorld:
	{
		uint16 childcmd;
		SIMPLE_PROTOCOLS_RECEIVE(SP_EnterWorld, childcmd);
		UE_LOG(MMOARPG, Display, TEXT("Recv SP_EnterWorld [childcmd:%d]"), childcmd);
		break;
	}
	case SP_RoleBaseInfo:
	{
		S_ROLE_O_BASE RoleBase;
		SIMPLE_PROTOCOLS_RECEIVE(SP_RoleBaseInfo, RoleBase);
		if(MapOtherCharacter.Contains(RoleBase.index))
		{
			UE_LOG(MMOARPG, Error, TEXT("recv SP_RoleBaseInfo"));
		}
		else
		{
			UE_LOG(MMOARPG, Display, TEXT("Recv SP_RoleBaseInfo [uid:%d]"), RoleBase.index);
			auto temp = GetWorld()->SpawnActor<ABladeIINetPlayer>(OtherCharacterClass);
			if (temp)
			{
				MapOtherCharacter.Emplace(RoleBase.index, temp);
				MapOtherCharacter[RoleBase.index]->UpdateBaseData(&RoleBase);
				//MapOtherCharacter[RoleBase.index]->UpdateBaseData(&RoleBase);
			}
			else
			{
				UE_LOG(MMOARPG, Error, TEXT("recv SP_RoleBaseInfo SpawnActor"));
			}
		}
		break;
	}
	}
}

//DS Server Timer
void ABladeIINetGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	////GThread::Get()->GetCoroutines().BindLambda(0.5f, [&](APlayerController* InNewPlayer)
	//{
	//	if (InNewPlayer)
	//	{
	//		/*
	//		if (AMMOARPGPlayerCharacter* InPawn = InNewPlayer->GetPawn<AMMOARPGPlayerCharacter>())
	//		{
	//			if (AMMOARPGGameState* InGameState = GetGameState<AMMOARPGGameState>())
	//			{
	//				if (FCharacterAnimTable *InAnimTable = InGameState->GetCharacterAnimTable(InPawn->GetID()))
	//				{
	//					InPawn->AnimTable = InAnimTable;
	//				}
	//			}
	//		}*/
	//	}
	//},NewPlayer);
}