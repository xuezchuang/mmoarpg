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
#include "MMOARPGMacroType.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"
#include "SocketSubsystem.h"
#include "Protocol/LoginProtocol.h"
#include "Protocol/HallProtocol.h"
#include "Protocol/GameProtocol.h"
#include "Channel/SimpleChannel.h"
#include "UObject/SimpleController.h"
//#include "SimpleAdvancedAnimationBPLibrary.h"

int UMMOARPGGameInstance::nIndex = 0;

namespace
{
	FString NormalizeMapName(const FString& InMapName)
	{
		FString ShortName = FPackageName::GetShortName(InMapName);
		TArray<FString> Parts;
		ShortName.ParseIntoArray(Parts, TEXT("_"), true);
		if (Parts.Num() >= 3 && Parts[0] == TEXT("UEDPIE") && Parts[1].IsNumeric())
		{
			FString Result = Parts[2];
			for (int32 Index = 3; Index < Parts.Num(); ++Index)
			{
				Result += TEXT("_") + Parts[Index];
			}
			return Result;
		}

		return ShortName;
	}
}

void UMMOARPGGameInstance::Init()
{	
	Super::Init();
	nIndex++;
	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] GameInstance Init [Enabled:%d]"), bEnableQuickTest ? 1 : 0);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMMOARPGGameInstance::HandleQuickTestMapLoaded);
	if (UWorld* World = GetWorld())
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Init TryStartQuickTestBootstrap [%s]"), *World->GetMapName());
		ResetQuickTestRuntimeState();
		TryStartQuickTestBootstrap(World);
	}
}

void UMMOARPGGameInstance::Tick(float DeltaTime)
{
	if(Client)
	{
		Client->Tick(DeltaTime);
	}

	if (bPendingEnterWorldAfterTravel && !bEnterWorldSentForCurrentTravel)
	{
		TrySendDeferredEnterWorld(GetWorld());
	}

	GThread::Get()->Tick(DeltaTime);
}

TStatId UMMOARPGGameInstance::GetStatId() const
{
	return TStatId();
}

void UMMOARPGGameInstance::Shutdown()
{
	StopQuickTestNetworkFlow();
	Super::Shutdown();
	nIndex--;
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
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

bool UMMOARPGGameInstance::CanSendGameplayProtocols() const
{
	if (!Client)
	{
		return false;
	}

	const UMMOARPGNetSubsystem* NetSub = GetSubsystem<UMMOARPGNetSubsystem>();
	if (!NetSub || NetSub->CurrentRole != ENetServerRole::Gate)
	{
		return false;
	}

	if (auto* Ctrl = Client->GetController())
	{
		if (FSimpleChannel* Channel = Ctrl->GetChannel())
		{
			if (TSharedPtr<FSimpleConnetion> Conn = Channel->GetConnetion())
			{
				return Conn->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure;
			}
		}
	}

	return false;
}

void UMMOARPGGameInstance::QueueEnterWorldAfterTravel()
{
	bPendingEnterWorldAfterTravel = true;
	bEnterWorldSentForCurrentTravel = false;
	UE_LOG(MMOARPG, Display, TEXT("[GameplayNet] Queue SP_EnterWorld after successful character response"));
}

void UMMOARPGGameInstance::TrySendDeferredEnterWorld(UWorld* LoadedWorld)
{
	if (!bPendingEnterWorldAfterTravel || bEnterWorldSentForCurrentTravel || !LoadedWorld)
	{
		return;
	}

	const FString CurrentMap = NormalizeMapName(LoadedWorld->GetMapName());
	if (CurrentMap == TEXT("Login") || CurrentMap == TEXT("HallMap"))
	{
		return;
	}

	if (!CanSendGameplayProtocols())
	{
		return;
	}

	bEnterWorldSentForCurrentTravel = true;
	bPendingEnterWorldAfterTravel = false;
	UE_LOG(MMOARPG, Display, TEXT("[GameplayNet] Deferred send SP_EnterWorld on map [%s]"), *CurrentMap);
	SIMPLE_CLIENT_SEND(GetClient(), SP_EnterWorld);
}

void UMMOARPGGameInstance::HandleQuickTestMapLoaded(UWorld* LoadedWorld)
{
	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] PostLoadMapWithWorld [%s] [Enabled:%d]"), LoadedWorld ? *LoadedWorld->GetMapName() : TEXT("None"), bEnableQuickTest ? 1 : 0);
	ResetQuickTestRuntimeState();
	TryStartQuickTestBootstrap(LoadedWorld);
	TrySendDeferredEnterWorld(LoadedWorld);
}

void UMMOARPGGameInstance::TryStartQuickTestBootstrap(UWorld* LoadedWorld)
{
	if (!bEnableQuickTest || !LoadedWorld)
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Skip bootstrap [Enabled:%d World:%s]"),
			bEnableQuickTest ? 1 : 0,
			LoadedWorld ? *LoadedWorld->GetMapName() : TEXT("None"));
		return;
	}

	const FString CurrentMap = NormalizeMapName(LoadedWorld->GetMapName());
	const bool bHasGateInfo = GateStatus.GateServerAddrInfo.Port != 0;
	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Evaluate bootstrap [Map:%s HasGate:%d Bootstrapping:%d Active:%d]"),
		*CurrentMap, bHasGateInfo ? 1 : 0, bQuickTestBootstrapping ? 1 : 0, bQuickTestNetworkFlowActive ? 1 : 0);

	if (CurrentMap == TEXT("Login"))
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Already in Login map"));
		return;
	}

	if (CurrentMap == TEXT("HallMap"))
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Skip bootstrap in HallMap"));
		return;
	}

	if (!bQuickTestBootstrapping && !bHasGateInfo)
	{
		bQuickTestBootstrapping = true;
		QuickTestStartupMap = CurrentMap;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Bootstrap from map [%s], start background login flow"), *QuickTestStartupMap);
		StartQuickTestNetworkFlow(LoadedWorld);
	}
	else
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Bootstrap already satisfied [Map:%s]"), *CurrentMap);
	}
}

FString UMMOARPGGameInstance::GetQuickTestTravelMap() const
{
	if (bEnableQuickTest && !QuickTestStartupMap.IsEmpty())
	{
		return QuickTestStartupMap;
	}

	return TEXT("TestInventory");
}

void UMMOARPGGameInstance::StartQuickTestNetworkFlow(UWorld* LoadedWorld)
{
	if (bQuickTestNetworkFlowActive)
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Network flow already active"));
		return;
	}

	if (!LoadedWorld)
	{
		return;
	}

	if (QuickTestStartupMap.IsEmpty())
	{
		QuickTestStartupMap = NormalizeMapName(LoadedWorld->GetMapName());
	}

	if (UMMOARPGNetSubsystem* NetSub = GetSubsystem<UMMOARPGNetSubsystem>())
	{
		bQuickTestLoginRequestSent = false;
		bQuickTestCharacterLoginSent = false;
		NetSub->RegisterUniqueHandlers(
			{
				SP_LoginResponses,
				SP_CharacterLogin,
				SP_CharacterSelect,
				SP_CharacterResponse
			},
			FProtocolHandler::CreateUObject(this, &UMMOARPGGameInstance::RecvQuickTestProtocol));

		NetSub->OnNetLinked.BindUObject(this, &UMMOARPGGameInstance::QuickTestLinkInit);
		bQuickTestNetworkFlowActive = true;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] BeginLink(Login) [TargetMap:%s Account:%s]"), *QuickTestStartupMap, *QuickTestAccount);
		NetSub->BeginLink(ENetServerRole::Login);
	}
}

void UMMOARPGGameInstance::StopQuickTestNetworkFlow()
{
	if (UMMOARPGNetSubsystem* NetSub = GetSubsystem<UMMOARPGNetSubsystem>())
	{
		NetSub->UnRegisterUniqueHandlers(
			{
				SP_LoginResponses,
				SP_CharacterLogin,
				SP_CharacterSelect,
				SP_CharacterResponse
			});

		if (bQuickTestNetworkFlowActive)
		{
			NetSub->OnNetLinked.Unbind();
		}
	}

	bQuickTestBootstrapping = false;
	bQuickTestNetworkFlowActive = false;
	bQuickTestLoginRequestSent = false;
	bQuickTestCharacterLoginSent = false;
}

void UMMOARPGGameInstance::ResetQuickTestRuntimeState()
{
	bQuickTestBootstrapping = false;
	bQuickTestNetworkFlowActive = false;
	bQuickTestLoginRequestSent = false;
	bQuickTestCharacterLoginSent = false;
	QuickTestStartupMap.Reset();
	GateStatus.GateServerAddrInfo.Port = 0;
}

void UMMOARPGGameInstance::QuickTestLinkInit(ENetServerRole ServerRole)
{
	if (!bQuickTestNetworkFlowActive)
	{
		return;
	}

	if (ServerRole == ENetServerRole::Login)
	{
		if (bQuickTestLoginRequestSent)
		{
			return;
		}

		S_LOGIN_NAME Name{};
		S_LOGIN_PASS Pass{};
		FMemory::Memcpy(&Name, TCHAR_TO_UTF8(*QuickTestAccount), USER_MAX_MEMBER);
		FMemory::Memcpy(&Pass, TCHAR_TO_UTF8(*QuickTestPassword), USER_MAX_PASS);
		FMemory::Memcpy(&UserData.Account, TCHAR_TO_UTF8(*QuickTestAccount), USER_MAX_MEMBER);
		bQuickTestLoginRequestSent = true;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Send SP_LoginResponses [Account:%s]"), *QuickTestAccount);
		SIMPLE_CLIENT_SEND(GetClient(), SP_LoginResponses, Name, Pass);
	}
	else if (ServerRole == ENetServerRole::Gate)
	{
		if (bQuickTestCharacterLoginSent)
		{
			return;
		}

		bQuickTestCharacterLoginSent = true;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Send SP_CharacterLogin [Account:%s]"), ANSI_TO_TCHAR(UserData.Account.name));
		SIMPLE_CLIENT_SEND(GetClient(), SP_CharacterLogin, UserData.Account, UserData.loingkey);
	}
}

void UMMOARPGGameInstance::RecvQuickTestProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_LoginResponses:
	{
		uint16 Childcmd = 0;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> Childcmd;
		if (Childcmd != 0)
		{
			UE_LOG(MMOARPG, Error, TEXT("[QuickTest] Recv SP_LoginResponses [childcmd:%d]"), Childcmd);
			StopQuickTestNetworkFlow();
			if (GetClient())
			{
				if (FSimpleChannel* ClientChannel = GetClient()->GetChannel())
				{
					UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Close connection after login failure [childcmd:%d]"), Childcmd);
					ClientChannel->DestroySelf();
				}
				else
				{
					GetClient()->Close();
				}
			}
			return;
		}

		S_LOGIN_KEY LoginKey{};
		S_LOGIN_IP IP{};
		uint16 Port = 0;
		Stream >> LoginKey >> IP >> Port;

		if (ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
		{
			TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
			bool bValid = false;
			const FString IPStr = UTF8_TO_TCHAR(IP.ip);
			Addr->SetIp(*IPStr, bValid);
			uint32 NumericIP = 0;
			Addr->GetIp(NumericIP);

			GateStatus.GateServerAddrInfo.IP = NumericIP;
			GateStatus.GateServerAddrInfo.Port = Port;
			UserData.loingkey = LoginKey;
			UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Recv SP_LoginResponses -> Gate [%s:%d]"), *IPStr, Port);
		}

		if (GetClient() && GetClient()->GetChannel())
		{
			GetClient()->GetChannel()->DestroySelf();
		}

		GThread::Get()->GetCoroutines().BindLambda(0.5f, [this]()
			{
				if (UMMOARPGNetSubsystem* NetSub = GetSubsystem<UMMOARPGNetSubsystem>())
				{
					UE_LOG(MMOARPG, Display, TEXT("[QuickTest] BeginLink(Gate)"));
					NetSub->BeginLink(ENetServerRole::Gate);
				}
			});
		break;
	}
	case SP_CharacterLogin:
	{
		uint16 Childcmd = 0;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> Childcmd;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Recv SP_CharacterLogin [childcmd:%d]"), Childcmd);
		if (Childcmd != 0)
		{
			return;
		}

		UserData.Reset();
		Stream >> UserData.ID >> UserData.role[0] >> UserData.role[1] >> UserData.role[2];

		const int32 Slot = ResolveQuickTestCharacterSlot();
		if (Slot == INDEX_NONE)
		{
			UE_LOG(MMOARPG, Error, TEXT("[QuickTest] No valid character slot found"));
			return;
		}

		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Queue SP_CharacterSelect [slot:%d mid:%lld]"), Slot, UserData.ID);
		GThread::Get()->GetCoroutines().BindLambda(0.2f, [this, Slot]()
			{
				UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Send SP_CharacterSelect [slot:%d mid:%lld]"), Slot, UserData.ID);
				SIMPLE_CLIENT_SEND(GetClient(), SP_CharacterSelect, Slot, UserData.ID);
			});
		break;
	}
	case SP_CharacterSelect:
	{
		uint16 Childcmd = 0;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> Childcmd;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Recv SP_CharacterSelect [childcmd:%d]"), Childcmd);
		break;
	}
	case SP_CharacterResponse:
	{
		uint8 Childcmd = 0;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> Childcmd;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Recv SP_CharacterResponse [childcmd:%d]"), Childcmd);
		if (Childcmd != 0)
		{
			return;
		}

		int32 UserIndex = 0;
		Stream >> UserData.base.exp >> UserData.base.econ >> UserData.base.status >> UserData.base.life;
		Stream >> UserIndex >> UserData.stand.myskill >> UserData.stand.bag;
		const FString TargetMap = GetQuickTestTravelMap();
		QueueEnterWorldAfterTravel();
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Character ready -> OpenLevel [%s]"), *TargetMap);
		StopQuickTestNetworkFlow();
		UGameplayStatics::OpenLevel(GetWorld(), FName(*TargetMap));
		break;
	}
	default:
		break;
	}
}

int32 UMMOARPGGameInstance::ResolveQuickTestCharacterSlot() const
{
	if (QuickTestCharacterSlot >= 0 &&
		QuickTestCharacterSlot < USER_MAX_ROLE &&
		UserData.role[QuickTestCharacterSlot].isT())
	{
		return QuickTestCharacterSlot;
	}

	for (int32 Slot = 0; Slot < USER_MAX_ROLE; ++Slot)
	{
		if (UserData.role[Slot].isT())
		{
			return Slot;
		}
	}

	return INDEX_NONE;
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
