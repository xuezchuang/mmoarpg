#pragma once

#include "CoreMinimal.h"
#include "MMOARPGEnemyController.h"
#include "MMOARPGNetEnemyController.generated.h"

// === 服务器下发的网络状态（可按需扩展） ===
UENUM(BlueprintType)
enum class ENetMonsterAction : uint8
{
    Idle,
    Run,
    Attack,
    Hit,
    Dead
};

USTRUCT(BlueprintType)
struct FNetMonsterState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector  Pos = FVector::ZeroVector;     // 世界坐标
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rot = FRotator::ZeroRotator;  // 朝向（一般只用 Yaw）
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float    Speed = 0.f;                  // 水平速度（可选）
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ENetMonsterAction Action = ENetMonsterAction::Idle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double   ServerTime = 0.0;             // 服务器时间（秒）
    //UPROPERTY(EditAnywhere, BlueprintReadWrite) uint32   Seq = 0;                      // 包序号（可选）
};

class AMMOARPGMonster;
/**
 *  网络版控制器：不做本地 AI，只做“网络状态 → 客户端平滑表现”
 *  重要：不要在 Monster 端调用 Patrol/OnAggroedPulled 等 AI 接口（这些属于单机控制器）
 */
UCLASS()
class MMOARPG_API AMMOARPGNetEnemyController : public AMMOARPGEnemyController
{
    GENERATED_BODY()


public:

	AMMOARPGNetEnemyController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // ========== 网络层喂数据的接口（你在接收包时调用） ==========
    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void QueueNetState(const FNetMonsterState& InState);

    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void QueueNetStates(const TArray<FNetMonsterState>& States);

    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void ClearStateBuffer();

    // 立即对齐（传送/大误差拉齐）
    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void SnapToState(const FNetMonsterState& InState, bool bAdjustZ = true);

    // ========== 可调参数 ==========
    // 插值延迟（越大越稳、越小越跟手），常见 0.08~0.2
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    float InterpDelaySeconds = 0.12f;

    // 偏差过大直接瞬移（cm）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    float SnapThreshold = 300.f;

    // 应用位置时是否贴地
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    bool bAdjustZOnApply = true;

    // 给 AnimBP 读的变量（按需绑定）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MMOARPG|Enemy|Net")
    float NetSpeed = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MMOARPG|Enemy|Net")
    ENetMonsterAction NetAction = ENetMonsterAction::Idle;

public:
	// 将“移动到某个世界坐标”的网络状态入队（无服务器时间戳时用本地时间）
	void Net_MoveTo(const FVector& Target, float Speed = 0.f, bool bChasing = false);

	// 如果你从 8400/其它包里拿到了“服务器时间戳”，用这个版本更准
	// 约定：ServerTimeSeconds 是“秒”为单位（和 StateBuffer 的 ServerTime 一致）
	void Net_MoveTo_At(const FVector& Target, double ServerTimeSeconds, float Speed = 0.f, bool bChasing = false);

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // === 内部：状态缓冲 & 插值 ===
    double ComputeClientRenderTime() const;
    void   InsertStateSorted(const FNetMonsterState& InState);
    bool   SampleStates(double ClientRenderTime, FNetMonsterState& OutPrev, FNetMonsterState& OutNext) const;
    void   ApplyInterpolated(AMMOARPGMonster* tEnemyPawn, const FNetMonsterState& A, const FNetMonsterState& B, double ClientRenderTime, float DeltaSeconds);

    // 贴地（复用父类已有逻辑签名不同，故再写一个静态工具）
    void   AdjustZToGround(AMMOARPGMonster* tEnemyPawn, FVector& InOutPos) const;

private:
    UPROPERTY() TArray<FNetMonsterState> StateBuffer;
    UPROPERTY(EditAnywhere, Category="MMOARPG|Enemy|Net") int32 MaxBufferSize = 64;

    FVector PrevVisualPos = FVector::ZeroVector;
    bool    bHasPrevVisualPos = false;
};
