// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGGameInstance.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "ThreadManage.h"
#include "DataTable/MonsterAnimTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "MMOARPGMonster.h"
#include "MMOARPGNetEnemyController.h"
#include "MMOARPTool.h"
#include "MMOARPG.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "Kismet/GameplayStatics.h"
//#include "SimpleAdvancedAnimationBPLibrary.h"

int UMMOARPGGameInstance::nIndex = 0;

void UMMOARPGGameInstance::Init()
{	
	Super::Init();
	nIndex++;


}

void UMMOARPGGameInstance::Tick(float DeltaTime)
{
	if(Client)
	{
		Client->Tick(DeltaTime);
	}

	GThread::Get()->Tick(DeltaTime);
}

TStatId UMMOARPGGameInstance::GetStatId() const
{
	return TStatId();
}

void UMMOARPGGameInstance::Shutdown()
{
	Super::Shutdown();
	nIndex--;
	if(Client)
	{
		FSimpleNetManage::Destroy(Client);
	}

	GThread::Destroy();
	//USimpleAdvancedAnimationBPLibrary::Destroy();
}

void UMMOARPGGameInstance::CreateClient()
{
	if (!Client)
	{
		FSimpleNetGlobalInfo::Get()->Init();

		Client = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	}
}

void UMMOARPGGameInstance::LinkLoginServer()
{
	if (Client)
	{
		if (!Client->Init())
		{
			delete Client;
			Client = NULL;
		}
	}
}

void UMMOARPGGameInstance::LinkGateServer()
{
	if (!Client)
	{
		return;
	}
	if (GateStatus.GateServerAddrInfo.Port == 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("LinkGateServer Failed: GateServerAddrInfo is not assigned yet. "
				"You must login and receive Gate info first."));
		return;
	}

	if (!Client->Init(GateStatus.GateServerAddrInfo))
	{
		delete Client;
		Client = NULL;
	}
}


FSimpleNetManage* UMMOARPGGameInstance::GetClient()
{
	return Client;
}

FMMOARPGUserData& UMMOARPGGameInstance::GetUserData()
{
	return UserData;
}

FMMOARPGGateStatus& UMMOARPGGameInstance::GetGateStatus()
{
	return GateStatus;
}


UDataTable* UMMOARPGGameInstance::EnsureMonsterTableSync()
{
    if (DT_Monster_Loaded) return DT_Monster_Loaded;
    if (DT_Monster.IsNull()) return nullptr;

    DT_Monster_Loaded = DT_Monster.LoadSynchronous(); // 同步加载
    return DT_Monster_Loaded;
}

const FMonsterAnimRow* UMMOARPGGameInstance::GetMonsterRowSync(int32 MonsterId)
{
    UDataTable* Table = EnsureMonsterTableSync();
    if (!Table) return nullptr;

    static const FString Context(TEXT("MonsterRowLookup"));
    const FName RowName(*FString::FromInt(MonsterId));
    return Table->FindRow<FMonsterAnimRow>(RowName, Context);
}

UDataTable* UMMOARPGGameInstance::EnsurePlayerTableSync()
{
    if (DT_Player_Loaded) return DT_Player_Loaded;
    if (DT_Player.IsNull()) return nullptr;

    DT_Player_Loaded = DT_Player.LoadSynchronous(); // 同步加载
    return DT_Player_Loaded;
}

const FCharacterAnimRow* UMMOARPGGameInstance::GetPlayerRowSync(int32 JobId)
{
    UDataTable* Table = EnsurePlayerTableSync();
    if (!Table) return nullptr;

    static const FString Context(TEXT("PlayerRowLookup"));
    const FName RowName(*FString::FromInt(JobId));
    return Table->FindRow<FCharacterAnimRow>(RowName, Context);
}