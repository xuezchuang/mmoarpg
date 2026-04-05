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
	bool TryParseJobIdFromRowName(const FName& RowName, int32& OutJobId)
	{
		const FString RowString = RowName.ToString();
		if (LexTryParseString(OutJobId, *RowString))
		{
			return true;
		}

		FString DigitsOnly;
		for (const TCHAR Ch : RowString)
		{
			if (FChar::IsDigit(Ch))
			{
				DigitsOnly.AppendChar(Ch);
			}
		}

		return !DigitsOnly.IsEmpty() && LexTryParseString(OutJobId, *DigitsOnly);
	}

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

	int32 ExtractPieInstanceIndex(const FString& InMapName)
	{
		FString ShortName = FPackageName::GetShortName(InMapName);
		TArray<FString> Parts;
		ShortName.ParseIntoArray(Parts, TEXT("_"), true);
		if (Parts.Num() >= 3 && Parts[0] == TEXT("UEDPIE") && Parts[1].IsNumeric())
		{
			return FCString::Atoi(*Parts[1]);
		}

		return INDEX_NONE;
	}

	FString ResolveQuickTestAccountForInstance(const FString& BaseAccount, int32 InstanceIndex)
	{
		if (BaseAccount.IsEmpty() || InstanceIndex <= 0)
		{
			return BaseAccount;
		}

		int32 SeparatorIndex = INDEX_NONE;
		if (!BaseAccount.FindLastChar(TEXT('_'), SeparatorIndex))
		{
			return BaseAccount;
		}

		const FString Prefix = BaseAccount.Left(SeparatorIndex + 1);
		const FString NumericSuffix = BaseAccount.Mid(SeparatorIndex + 1);
		if (!NumericSuffix.IsNumeric())
		{
			return BaseAccount;
		}

		const int32 BaseIndex = FCString::Atoi(*NumericSuffix);
		return FString::Printf(TEXT("%s%d"), *Prefix, BaseIndex + InstanceIndex);
	}

	bool DoesWorldBelongToGameInstance(const UWorld* World, const UGameInstance* GameInstance)
	{
		return World && GameInstance && World->GetGameInstance() == GameInstance;
	}

	UDataTable* TryLoadDataTableFromCandidates(const TArray<const TCHAR*>& CandidatePaths)
	{
		for (const TCHAR* CandidatePath : CandidatePaths)
		{
			if (!CandidatePath || CandidatePath[0] == '\0')
			{
				continue;
			}

			if (UDataTable* Table = LoadObject<UDataTable>(nullptr, CandidatePath))
			{
				return Table;
			}
		}

		return nullptr;
	}

	bool ShouldAutoEnableQuickTestForWorld(const UWorld* World)
	{
		return World && World->WorldType == EWorldType::PIE;
	}
}

void UMMOARPGGameInstance::Init()
{	
	Super::Init();
	nIndex++;
	const bool bEffectiveQuickTestEnabled = bEnableQuickTest || ShouldAutoEnableQuickTestForWorld(GetWorld());
	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] GameInstance Init [Enabled:%d Effective:%d]"),
		bEnableQuickTest ? 1 : 0, bEffectiveQuickTestEnabled ? 1 : 0);

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

	if (bPendingEnterWorldAfterTravel && !bEnterWorldSentForCurrentTravel && !bWaitingForEnterWorldMapLoad)
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
	bWaitingForEnterWorldMapLoad = true;
	UE_LOG(MMOARPG, Display, TEXT("[GameplayNet] Queue SP_EnterWorld after successful character response"));
}

void UMMOARPGGameInstance::TrySendDeferredEnterWorld(UWorld* LoadedWorld)
{
	if (!bPendingEnterWorldAfterTravel || bEnterWorldSentForCurrentTravel || !LoadedWorld)
	{
		return;
	}

	if (!DoesWorldBelongToGameInstance(LoadedWorld, this))
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
	bWaitingForEnterWorldMapLoad = false;
	UE_LOG(MMOARPG, Display, TEXT("[GameplayNet] Deferred send SP_EnterWorld on map [%s]"), *CurrentMap);
	SIMPLE_CLIENT_SEND(GetClient(), SP_EnterWorld);
}

void UMMOARPGGameInstance::HandleQuickTestMapLoaded(UWorld* LoadedWorld)
{
	if (!DoesWorldBelongToGameInstance(LoadedWorld, this))
	{
		return;
	}

	const bool bEffectiveQuickTestEnabled = bEnableQuickTest || ShouldAutoEnableQuickTestForWorld(LoadedWorld);
	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] PostLoadMapWithWorld [%s] [Enabled:%d Effective:%d]"),
		LoadedWorld ? *LoadedWorld->GetMapName() : TEXT("None"), bEnableQuickTest ? 1 : 0, bEffectiveQuickTestEnabled ? 1 : 0);

	if (bPendingEnterWorldAfterTravel)
	{
		bWaitingForEnterWorldMapLoad = false;
		TrySendDeferredEnterWorld(LoadedWorld);
		return;
	}

	ResetQuickTestRuntimeState();
	TryStartQuickTestBootstrap(LoadedWorld);
	TrySendDeferredEnterWorld(LoadedWorld);
}

void UMMOARPGGameInstance::TryStartQuickTestBootstrap(UWorld* LoadedWorld)
{
	const bool bEffectiveQuickTestEnabled = bEnableQuickTest || ShouldAutoEnableQuickTestForWorld(LoadedWorld);
	if (!bEffectiveQuickTestEnabled || !LoadedWorld)
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Skip bootstrap [Enabled:%d Effective:%d World:%s]"),
			bEnableQuickTest ? 1 : 0,
			bEffectiveQuickTestEnabled ? 1 : 0,
			LoadedWorld ? *LoadedWorld->GetMapName() : TEXT("None"));
		return;
	}

	if (!bEnableQuickTest && ShouldAutoEnableQuickTestForWorld(LoadedWorld))
	{
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Auto-enable bootstrap for PIE world [%s]"), *LoadedWorld->GetMapName());
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
	if (!QuickTestStartupMap.IsEmpty())
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

	ResolveQuickTestRuntimeCredentials(LoadedWorld);

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
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] BeginLink(Login) [TargetMap:%s Account:%s Instance:%d]"),
			*QuickTestStartupMap, *QuickTestResolvedAccount, QuickTestRuntimeInstanceIndex);
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
	bWaitingForEnterWorldMapLoad = false;
	QuickTestLoginRetryCount = 0;
	QuickTestRuntimeInstanceIndex = 0;
	QuickTestResolvedAccount.Reset();
	QuickTestResolvedPassword.Reset();
	QuickTestStartupMap.Reset();
	GateStatus.GateServerAddrInfo.Port = 0;
	LocalUserIndex = INDEX_NONE;
	CurrentCharacterSlot = INDEX_NONE;
}

void UMMOARPGGameInstance::ResolveQuickTestRuntimeCredentials(UWorld* LoadedWorld)
{
	if (!QuickTestResolvedAccount.IsEmpty())
	{
		return;
	}

	int32 InstanceIndex = INDEX_NONE;
	if (LoadedWorld)
	{
		InstanceIndex = ExtractPieInstanceIndex(LoadedWorld->GetMapName());
	}

	if (InstanceIndex == INDEX_NONE)
	{
		InstanceIndex = FMath::Max(0, nIndex - 1);
	}

	QuickTestRuntimeInstanceIndex = InstanceIndex;
	QuickTestResolvedAccount = ResolveQuickTestAccountForInstance(QuickTestAccount, InstanceIndex);
	QuickTestResolvedPassword = QuickTestPassword;

	if (QuickTestResolvedAccount.IsEmpty())
	{
		QuickTestResolvedAccount = QuickTestAccount;
	}

	if (QuickTestResolvedPassword.IsEmpty())
	{
		QuickTestResolvedPassword = QuickTestPassword;
	}

	UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Resolved credentials [Base:%s Resolved:%s Instance:%d]"),
		*QuickTestAccount, *QuickTestResolvedAccount, QuickTestRuntimeInstanceIndex);
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
		FMemory::Memcpy(&Name, TCHAR_TO_UTF8(*QuickTestResolvedAccount), USER_MAX_MEMBER);
		FMemory::Memcpy(&Pass, TCHAR_TO_UTF8(*QuickTestResolvedPassword), USER_MAX_PASS);
		FMemory::Memcpy(&UserData.Account, TCHAR_TO_UTF8(*QuickTestResolvedAccount), USER_MAX_MEMBER);
		bQuickTestLoginRequestSent = true;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Send SP_LoginResponses [Account:%s]"), *QuickTestResolvedAccount);
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
		constexpr int32 MaxQuickTestLoginRetries = 12;
		constexpr float QuickTestLoginRetryDelaySeconds = 5.5f;
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

			if (Childcmd == 1003 && bEnableQuickTest && QuickTestLoginRetryCount < MaxQuickTestLoginRetries)
			{
				++QuickTestLoginRetryCount;
				UE_LOG(MMOARPG, Warning, TEXT("[QuickTest] Retry login after childcmd 1003 [attempt:%d/%d delay:%.1fs]"),
					QuickTestLoginRetryCount, MaxQuickTestLoginRetries, QuickTestLoginRetryDelaySeconds);
				GThread::Get()->GetCoroutines().BindLambda(QuickTestLoginRetryDelaySeconds, [this]()
					{
						if (!bEnableQuickTest)
						{
							return;
						}

						if (QuickTestStartupMap.IsEmpty())
						{
							if (UWorld* World = GetWorld())
							{
								QuickTestStartupMap = NormalizeMapName(World->GetMapName());
							}
						}

						bQuickTestBootstrapping = true;
						StartQuickTestNetworkFlow(GetWorld());
					});
			}
			return;
		}

		QuickTestLoginRetryCount = 0;

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

		const uint8 SlotId = static_cast<uint8>(Slot);
		SetCurrentCharacterSlot(Slot);
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Queue SP_CharacterSelect [slot:%d mid:%lld]"), SlotId, UserData.ID);
		GThread::Get()->GetCoroutines().BindLambda(0.2f, [this, SlotId]()
			{
				UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Send SP_CharacterSelect [slot:%d mid:%lld]"), SlotId, UserData.ID);
				SIMPLE_CLIENT_SEND(GetClient(), SP_CharacterSelect, SlotId, UserData.ID);
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
		uint16 Childcmd = 0;
		TArray<uint8> Buffer;
		Channel->Receive(Buffer);
		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));
		Stream >> Childcmd;
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] Recv SP_CharacterResponse [childcmd:%u]"), Childcmd);
		if (Childcmd != 0)
		{
			return;
		}

		int32 UserIndex = 0;
		Stream >> UserData.base.exp >> UserData.base.econ >> UserData.base.status >> UserData.base.life;
		Stream >> UserIndex >> UserData.stand.myskill >> UserData.stand.bag;
		SetLocalUserIndex(UserIndex);
		UE_LOG(MMOARPG, Display, TEXT("[QuickTest] CharacterResponse payload [userindex:%d level:%d gold:%d map:%d hp:%d mp:%d]"),
			UserIndex, UserData.base.exp.level, UserData.base.econ.gold, UserData.base.status.mapid, UserData.base.life.hp, UserData.base.life.mp);
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
    if (DT_Monster.IsNull())
    {
        static const TArray<const TCHAR*> MonsterTableCandidates =
        {
            TEXT("/Game/DataTable/Demo/DT_Monster.DT_Monster")
        };
        DT_Monster_Loaded = TryLoadDataTableFromCandidates(MonsterTableCandidates);
        return DT_Monster_Loaded;
    }
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
    if (DT_Player.IsNull())
    {
        static const TArray<const TCHAR*> PlayerTableCandidates =
        {
            TEXT("/Game/DataTable/Character/01/Character_01.Character_01"),
            TEXT("/Game/DataTable/Character/01/Character_1.Character_1"),
            TEXT("/Game/DataTable/Character/16424/Character_16424.Character_16424"),
            TEXT("/Game/DataTable/Character/16424/Character_016424.Character_016424")
        };
        DT_Player_Loaded = TryLoadDataTableFromCandidates(PlayerTableCandidates);
        return DT_Player_Loaded;
    }
    if (DT_Player.IsNull()) return nullptr;

    DT_Player_Loaded = DT_Player.LoadSynchronous(); // 同步加载
    return DT_Player_Loaded;
}

const FCharacterAnimRow* UMMOARPGGameInstance::GetPlayerRowSync(int32 JobId)
{
    UDataTable* Table = EnsurePlayerTableSync();
    if (!Table)
    {
		UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] DT_Player is null when resolving job [%d]"), JobId);
		return nullptr;
    }

    static const FString Context(TEXT("PlayerRowLookup"));
    const FName RowName(*FString::FromInt(JobId));
	if (const FCharacterAnimRow* ExactRow = Table->FindRow<FCharacterAnimRow>(RowName, Context))
	{
		return ExactRow;
	}

	const TArray<FName> RowNames = Table->GetRowNames();
	for (const FName& CandidateName : RowNames)
	{
		int32 CandidateJobId = INDEX_NONE;
		if (TryParseJobIdFromRowName(CandidateName, CandidateJobId) && CandidateJobId == JobId)
		{
			UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Fallback row match for job [%d] -> row [%s]"),
				JobId, *CandidateName.ToString());
			return Table->FindRow<FCharacterAnimRow>(CandidateName, Context);
		}
	}

	if (JobId == 0 && RowNames.Num() > 0)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Use default player row for job [0] -> row [%s]"),
			*RowNames[0].ToString());
		return Table->FindRow<FCharacterAnimRow>(RowNames[0], Context);
	}

	UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] No player row found for job [%d] in DT_Player [%s]"),
		JobId, *GetNameSafe(Table));
    return nullptr;
}
