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
#include "MMOARPTool.h"
#include "MMOARPG.h"
#include "Character/MMOARPGMonster.h"
#include "MMOARPGNetEnemyController.h"

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

// GameMode.cpp: 在 RecvProtocol 里新增怪物分支
void AMMOARPGGameMode::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
    switch (ProtocolNumber)
    {
    // ================= Monster =================
    case SP_MonsterData:
    {
		FMonsterDataPacket MonsterData;
		SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterData, MonsterData);
        if (MonsterData.ChildCmd != 0)
        {
            UE_LOG(MMOARPG, Warning, TEXT("MonsterData childcmd != 0 [%d]"), MonsterData.ChildCmd );
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

    //case SP_MonsterMove:
    //{
    //    // 8400：最低限——读 Id + grid_pos(+ 可选速度)
    //    uint32 Id = 0; int32 GridX = 0, GridY = 0; float Speed = 200.f;
    //    (*Channel) >> Id;
    //    (*Channel) >> GridX; (*Channel) >> GridY;
    //    // 如果服务端也发速度/是否追击，就再读：
    //    // (*Channel) >> Speed;

    //    AsyncTask(ENamedThreads::GameThread, [this, Id, GridX, GridY, Speed]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)Id))
    //        {
    //            Ctl->Net_MoveTo(GridToWorld(GridX, GridY), Speed, /*bChasing*/false);
    //        }
    //    });
    //    break;
    //}

    //case SP_MonsterHP:
    //{
    //    uint32 Id = 0; int32 CurHP = 0; int32 MaxHP = 0;
    //    (*Channel) >> Id >> CurHP >> MaxHP;

    //    AsyncTask(ENamedThreads::GameThread, [this, Id, CurHP, MaxHP]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)Id))
    //        {
    //            Ctl->Net_HealthUpdate(CurHP, MaxHP);
    //        }
    //    });
    //    break;
    //}

    //case SP_MonsterState:
    //{
    //    uint32 Id = 0; uint8 NewState = 0; // 自定义：0=Idle,1=Patrol,2=Chase,3=Back...
    //    (*Channel) >> Id >> NewState;

    //    AsyncTask(ENamedThreads::GameThread, [this, Id, NewState]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)Id))
    //        {
    //            // 仅表现：例如回家
    //            if (NewState == 3 /*Back*/)
    //            {
    //                if (AMMOARPGMonster* M = Cast<AMMOARPGMonster>(Ctl->GetPawn()))
    //                    Ctl->Net_ResetToHome(M->StartLocation);
    //            }
    //        }
    //    });
    //    break;
    //}

    //case SP_MonsterAttack:
    //{
    //    uint32 Id = 0; int32 AttackIndex = 0; float PlayRate = 1.f; float ExpectedSecs = 0.f;
    //    (*Channel) >> Id >> AttackIndex >> PlayRate >> ExpectedSecs;

    //    AsyncTask(ENamedThreads::GameThread, [this, Id, AttackIndex, PlayRate, ExpectedSecs]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)Id))
    //        {
    //            Ctl->Net_PlayAttack(AttackIndex, PlayRate, ExpectedSecs);
    //        }
    //    });
    //    break;
    //}

    //case SP_MonsterSkillDamage:
    //{
    //    // 8700：通常包含 施法者Id、受击者Id、伤害值、是否暴击 等
    //    uint32 CasterId=0, TargetId=0; int32 Dmg=0; uint8 bCrit=0;
    //    (*Channel) >> CasterId >> TargetId >> Dmg >> bCrit;
    //    // 这里只做受击飘字/受击姿势；真正血量变化交 8200
    //    AsyncTask(ENamedThreads::GameThread, [this, TargetId, Dmg, bCrit]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)TargetId))
    //        {
    //            if (AMMOARPGMonster* M = Cast<AMMOARPGMonster>(Ctl->GetPawn()))
    //            {
    //                M->PlayHitReact(/*方向*/);
    //                M->ShowFloatingText(-Dmg, bCrit != 0);
    //            }
    //        }
    //    });
    //    break;
    //}

    //case SP_MonsterBuff:
    //{
    //    // 8800：按你实际定义解析，添加/移除/刷新
    //    uint32 Id=0; uint16 BuffId=0; uint8 Op=0; float TimeLeft=0.f;
    //    (*Channel) >> Id >> BuffId >> Op >> TimeLeft;

    //    AsyncTask(ENamedThreads::GameThread, [this, Id, BuffId, Op, TimeLeft]()
    //    {
    //        if (AMMOARPGNetEnemyController* Ctl = FindMonsterCtlr((int32)Id))
    //        {
    //            if (AMMOARPGMonster* M = Cast<AMMOARPGMonster>(Ctl->GetPawn()))
    //            {
    //                M->UpdateBuffUI(BuffId, Op, TimeLeft);
    //            }
    //        }
    //    });
    //    break;
    //}

    // ===== 你原有的玩家/其他 =====
    default:
        break;
    }
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

// MMOARPGGameMode.cpp
void AMMOARPGGameMode::RegisterMonster(int32 Id, AMMOARPGNetEnemyController* C)
{
    MonsterMap.Add(Id, C);
}

void AMMOARPGGameMode::UnregisterMonster(int32 Id)
{
    MonsterMap.Remove(Id);
}

AMMOARPGNetEnemyController* AMMOARPGGameMode::FindMonsterCtlr(int32 Id) const
{
    if (const TWeakObjectPtr<AMMOARPGNetEnemyController>* P = MonsterMap.Find(Id))
    {
        return P->Get();
    }
    return nullptr;
}
