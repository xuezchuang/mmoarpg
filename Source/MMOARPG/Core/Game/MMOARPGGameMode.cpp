// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMOARPGGameMode.h"
#include "Character/MMOARPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MMOARPGHUD.h"
#include "ThreadManage.h"
#include "UObject/SimpleController.h"
#include "MMOARPGMacroType.h"
#include "MMOARPGPlayerState.h"
#include "MMOARPGGameState.h"
#include "ThreadManage.h"
#include "Character/MMOARPGPlayerCharacter.h"
#include "Protocol/GameProtocol.h"
#include "Core/MethodUnit.h"
#include "MMOARPGPlayerController.h"

AMMOARPGGameMode::AMMOARPGGameMode()
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

void AMMOARPGGameMode::BeginPlay()
{
	Super::BeginPlay();

	LinkServer();
}

void AMMOARPGGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
void AMMOARPGGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMMOARPGGameMode::LoginCharacterUpdateKneadingRequest(int32 InUserID)
{
	//SEND_DATA(SP_UpdateLoginCharacterInfoRequests,InUserID);
}

void AMMOARPGGameMode::BindClientRcv()
{
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (InGameInstance->GetClient() && InGameInstance->GetClient()->GetController())
		{
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

void AMMOARPGGameMode::LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
{

}

void AMMOARPGGameMode::LinkServer()
{
	//创建客户端
	if (UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	{
		InGameInstance->CreateClient();
		if (InGameInstance->GetClient())
		{
			InGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &AMMOARPGGameMode::LinkServerInfo);

			InGameInstance->LinkServer();
			//
			//作为测试
			//InGameInstance->LinkServer(TEXT("127.0.0.1"),11231);

			BindClientRcv();
		}
	}
}

void AMMOARPGGameMode::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	//switch (ProtocolNumber)
	//{
	//	case SP_UpdateLoginCharacterInfoResponses:
	//	{
	//		int32 UserID = INDEX_NONE;
	//		FString CAJsonString;
	//		SIMPLE_PROTOCOLS_RECEIVE(SP_UpdateLoginCharacterInfoResponses, UserID, CAJsonString);

	//		if (UserID != INDEX_NONE && !CAJsonString.IsEmpty())
	//		{
	//			//寻找特定玩家
	//			MethodUnit::ServerCallAllPlayerController<AMMOARPGPlayerController>(GetWorld(), [&](AMMOARPGPlayerController *InController)->MethodUnit::EServerCallType
	//			{
	//				if (AMMOARPGPlayerCharacter *InPlayerCharacter = InController->GetPawn<AMMOARPGPlayerCharacter>())
	//				{
	//					if (InPlayerCharacter->GetUserID() == UserID)
	//					{
	//						if (AMMOARPGPlayerState* InPlayerState = InController->GetPlayerState<AMMOARPGPlayerState>())		
	//						{
	//							NetDataAnalysis::StringToCharacterAppearances(CAJsonString,InPlayerState->GetCA());

	//							InPlayerCharacter->UpdateKneadingBoby(InPlayerState->GetCA());
	//							InPlayerCharacter->CallUpdateKneadingBobyOnClient(InPlayerState->GetCA());
	//						}

	//						return MethodUnit::EServerCallType::PROGRESS_COMPLETE;
	//					}
	//				}

	//				return MethodUnit::EServerCallType::INPROGRESS;					
	//			});
	//		}

	//		break;
	//	}
	//}
}

//DS Server Timer
void AMMOARPGGameMode::PostLogin(APlayerController* NewPlayer)
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