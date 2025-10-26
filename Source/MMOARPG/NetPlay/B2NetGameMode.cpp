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
#include "MMOARPTool.h"
#include "MMOARPG.h"
#include "Character/MMOARPGMonster.h"
#include "MMOARPGNetEnemyController.h"

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
	case SP_MonsterData:
	{
		FMonsterDataPacket MonsterData;
		SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterData, MonsterData);
		if (MonsterData.ChildCmd != 0)
		{
			UE_LOG(MMOARPG, Warning, TEXT("MonsterData childcmd != 0 [%d]"), MonsterData.ChildCmd);
		}

		if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr(MonsterData.Id))
		{

			/// 已存在：更新HP、位置/朝向
			if (AMMOARPGMonster* M = Cast<AMMOARPGMonster>(Ctl->GetPawn()))
			{
				//const FVector Loc = GridToWorld(GridX, GridY);
				//Ctl->Net_MoveTo(Loc, /*Speed*/200.f, /*bChasing*/false); // 速度先用小步速；后续服务端发 8400 修正
				//Ctl->Net_HealthUpdate((int32)Hp, M->TotalHealth);        // 或者你也在 8000 里带 MaxHP
				//// 简单朝向：用网格下一步方向近似
				//const FVector FacePos = Loc + FVector(FMath::Cos(FMath::DegreesToRadians((float)Dir)), FMath::Sin(FMath::DegreesToRadians((float)Dir)), 0.f) * 10.f;
				//Ctl->Net_FaceTo(FacePos);
				UE_LOG(MMOARPG, Display, TEXT("MonsterData"));
			}
		}
		else
		{
			FS_GRID_BASE Grid;
			Grid.row = MonsterData.GridX;
			Grid.col = MonsterData.GridY;

			// 计算世界坐标
			FVector WorldPos = UMMOARPTool::GridToPosSimple(Grid, FVector::ZeroVector, C_WORLDMAP_ONE_GRID, true);

			if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
			{
				// 调用 GameInstance 中的生成函数
				AMMOARPGMonster* SpawnedMonster = GI->SpawnMonsterByIdSync(MonsterData.Id, WorldPos);
				if (SpawnedMonster)
				{
					// 获取该怪物的控制器
					AController* Controller = SpawnedMonster->GetController();
					if (AMMOARPGNetEnemyController* NetController = Cast<AMMOARPGNetEnemyController>(Controller))
					{
						RegisterMonster(MonsterData.Id, NetController);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawned monster (ID:%lld) has no valid NetEnemyController."), MonsterData.Id);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to spawn monster with ID:%lld"), MonsterData.Id);
				}
			}

			//// 初始表现：HP、站位、朝向
			//NewCtl->Net_HealthUpdate((int32)Hp, Monster->TotalHealth);
			//NewCtl->Net_ResetToHome(SpawnLoc);
			//NewCtl->Net_FaceTo(SpawnLoc + SpawnRot.Vector() * 10.f);
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

void ABladeIINetGameMode::RegisterMonster(int32 Id, AMMOARPGNetEnemyController* C)
{
	MonsterMap.Add(Id, C);
}

void ABladeIINetGameMode::UnregisterMonster(int32 Id)
{
	MonsterMap.Remove(Id);
}

AMMOARPGNetEnemyController* ABladeIINetGameMode::FindMonsterCtlr(int32 Id) const
{
	if (const TWeakObjectPtr<AMMOARPGNetEnemyController>* P = MonsterMap.Find(Id))
	{
		return P->Get();
	}
	return nullptr;
}