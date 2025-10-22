// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGGameInstance.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "ThreadManage.h"
#include "DataTable/MonsterAnimTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "MMOARPGMonster.h"

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

void UMMOARPGGameInstance::LinkServer()
{
	if(Client)
	{
		if(!Client->Init())
		{
			delete Client;
			Client = NULL;
		}
	}
}

void UMMOARPGGameInstance::LinkServer(const FSimpleAddr& InAddr)
{
	if (Client)
	{
		if (!Client->Init(InAddr))
		{
			delete Client;
			Client = NULL;
		}
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

void UMMOARPGGameInstance::LinkServer(const TCHAR* InIP, uint32 InPort)
{
	if(Client)
	{
		if(!Client->Init(InIP, InPort))
		{
			delete Client;
			Client = NULL;
		}
	}
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

static UClass* ResolveAnimBPClassSync(const TSoftObjectPtr<UAnimBlueprint>& SoftBP)
{
    if (SoftBP.IsNull()) return nullptr;
    if (SoftBP.IsValid())
    {
        return SoftBP.Get()->GeneratedClass;
    }
    UAnimBlueprint* BP = SoftBP.LoadSynchronous();        // 同步加载 AnimBP 资产
    return BP ? BP->GeneratedClass : nullptr;
}

bool UMMOARPGGameInstance::GetMonsterVisualSync(int32 MonsterId, FMonsterVisualResolved& Out)
{
    const auto* Row = GetMonsterRowSync(MonsterId);
    if (!Row) return false;

    // Mesh
    if (Row->Mesh.IsNull()) return false;
    USkeletalMesh* Mesh = Row->Mesh.IsValid() ? Row->Mesh.Get()
                                              : Row->Mesh.LoadSynchronous();
    if (!Mesh) return false;
    Out.SkeletalMesh = Mesh;

    // AnimBP → Class
    Out.AnimClass = ResolveAnimBPClassSync(Row->AnimBlueprint);

    // Attack montages
    Out.AttackMontages.Reset();
    Out.AttackMontages.Reserve(Row->AttackMontages.Num());
    for (const auto& SoftM : Row->AttackMontages)
    {
        UAnimMontage* M = SoftM.IsValid() ? SoftM.Get() : SoftM.LoadSynchronous();
        if (M) Out.AttackMontages.Add(M);
    }

    // Optional single montages
    Out.Idle  = Row->Idle.IsNull()  ? nullptr : (Row->Idle.IsValid()  ? Row->Idle.Get()  : Row->Idle.LoadSynchronous());
    Out.Hit   = Row->Hit.IsNull()   ? nullptr : (Row->Hit.IsValid()   ? Row->Hit.Get()   : Row->Hit.LoadSynchronous());
    Out.Death = Row->Death.IsNull() ? nullptr : (Row->Death.IsValid() ? Row->Death.Get() : Row->Death.LoadSynchronous());

    return true;
}

void UMMOARPGGameInstance::GetMonsterVisualAsync(
    int32 MonsterId,
    TFunction<void(bool bOk, const FMonsterAnimRow* Row, const FMonsterVisualResolved& Visual)> OnReady)
{
    FMonsterVisualResolved Result;

    UDataTable* Table = EnsureMonsterTableSync();
    if (!Table)
    {
        if (OnReady) OnReady(false, nullptr, Result);
        return;
    }

    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row)
    {
        if (OnReady) OnReady(false, nullptr, Result);
        return;
    }

    // 收集需要异步加载的路径
	TArray<FSoftObjectPath> ToLoad;
	if (!Row->MonsterBlueprint.IsValid())
		ToLoad.Add(Row->MonsterBlueprint.ToSoftObjectPath());
	if (!Row->Mesh.IsValid())            
		ToLoad.Add(Row->Mesh.ToSoftObjectPath());
	if (!Row->AnimBlueprint.IsValid())  
		ToLoad.Add(Row->AnimBlueprint.ToSoftObjectPath());

    for (const auto& SoftM : Row->AttackMontages)
        if (!SoftM.IsValid()) ToLoad.Add(SoftM.ToSoftObjectPath());

    if (!Row->Idle.IsNull()  && !Row->Idle.IsValid())   ToLoad.Add(Row->Idle.ToSoftObjectPath());
    if (!Row->Hit.IsNull()   && !Row->Hit.IsValid())    ToLoad.Add(Row->Hit.ToSoftObjectPath());
    if (!Row->Death.IsNull() && !Row->Death.IsValid())  ToLoad.Add(Row->Death.ToSoftObjectPath());

    auto FinishResolve = [this, Row, OnReady](FMonsterVisualResolved ResultResolved)
    {
        if (OnReady) OnReady(true, Row, ResultResolved);
    };

    if (ToLoad.Num() == 0)
    {
        // 都已在内存，直接组装
        GetMonsterVisualSync(MonsterId, Result);
        FinishResolve(Result);
        return;
    }

    // 异步加载
    FStreamableManager& SM = UAssetManager::GetStreamableManager();
    SM.RequestAsyncLoad(ToLoad, [this, MonsterId, Row, FinishResolve]()
    {
        FMonsterVisualResolved R;
        const bool bOk = GetMonsterVisualSync(MonsterId, R);
        if (!bOk)
        {
            FMonsterVisualResolved Empty;
            FinishResolve(Empty);
            return;
        }
        FinishResolve(R);
    });
}

void UMMOARPGGameInstance::SpawnMonsterByIdAsync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    // 先把蓝图类也加入到 GetMonsterVisualAsync 的加载队列里（见下方改造）
    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: Row not found id=%d"), MonsterId);
        return;
    }

    GetMonsterVisualAsync(MonsterId,
        [this, MonsterId, Pos, Rot](bool bOk, const FMonsterAnimRow* ResolvedRow, const FMonsterVisualResolved& Visual)
        {
            if (!bOk || !ResolvedRow)
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: Visual resolve failed id=%d"), MonsterId);
                return;
            }

            UWorld* W = GetWorld();
            if (!W) return;

            // 蓝图类也应已加载（见下方修改）
            UClass* BPClass = ResolvedRow->MonsterBlueprint.Get();
            if (!BPClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: MonsterBlueprint not ready id=%d"), MonsterId);
                return;
            }

            AMMOARPGMonster* Monster = W->SpawnActor<AMMOARPGMonster>(BPClass, Pos, Rot);
            if (!Monster) return;

            Monster->MonsterID = MonsterId;

            if (USkeletalMeshComponent* Skel = Monster->GetMesh())
            {
                if (Visual.SkeletalMesh) Skel->SetSkeletalMesh(Visual.SkeletalMesh);
                if (Visual.AnimClass)    Skel->SetAnimInstanceClass(Visual.AnimClass);
            }

            // 这里可把 Visual.AttackMontages/Idle/Hit/Death 注册到 Monster 的动作系统里
            Monster->UpdateHealthBar();
        });
}

AMMOARPGMonster* UMMOARPGGameInstance::SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdSync: Row not found for id=%d"), MonsterId);
        return nullptr;
    }

    // 同步加载（会阻塞）—— 小表/编辑器可用
    if (!Row->MonsterBlueprint.IsValid())
        Row->MonsterBlueprint.LoadSynchronous();
    if (!Row->Mesh.IsValid())
        Row->Mesh.LoadSynchronous();
    if (!Row->AnimBlueprint.IsValid())
        Row->AnimBlueprint.LoadSynchronous();
    for (auto& M : const_cast<TArray<TSoftObjectPtr<UAnimMontage>>&>(Row->AttackMontages))
        if (!M.IsValid()) M.LoadSynchronous();
    if (!Row->Idle.IsValid())  Row->Idle.LoadSynchronous();
    if (!Row->Hit.IsValid())   Row->Hit.LoadSynchronous();
    if (!Row->Death.IsValid()) Row->Death.LoadSynchronous();

    UClass* BPClass = Row->MonsterBlueprint.Get();
    if (!BPClass) return nullptr;

    AMMOARPGMonster* Monster = World->SpawnActor<AMMOARPGMonster>(BPClass, Pos, Rot);
    if (!Monster) return nullptr;

    Monster->MonsterID = MonsterId;

    if (USkeletalMeshComponent* Skel = Monster->GetMesh())
    {
        if (USkeletalMesh* Mesh = Row->Mesh.Get())
            Skel->SetSkeletalMesh(Mesh);
        if (UAnimBlueprint* AnimBP = Row->AnimBlueprint.Get())
            if (UClass* AnimClass = AnimBP->GeneratedClass)
                Skel->SetAnimInstanceClass(AnimClass);
    }

    Monster->UpdateHealthBar();
    return Monster;
}