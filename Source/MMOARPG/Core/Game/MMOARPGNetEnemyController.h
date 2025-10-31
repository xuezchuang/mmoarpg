#pragma once

#include "CoreMinimal.h"
#include "MMOARPGEnemyController.h"
#include "MMOARPGNetEnemyController.generated.h"

// === �������·�������״̬���ɰ�����չ�� ===
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector  Pos = FVector::ZeroVector;     // ��������
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rot = FRotator::ZeroRotator;  // ����һ��ֻ�� Yaw��
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float    Speed = 0.f;                  // ˮƽ�ٶȣ���ѡ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ENetMonsterAction Action = ENetMonsterAction::Idle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double   ServerTime = 0.0;             // ������ʱ�䣨�룩
    //UPROPERTY(EditAnywhere, BlueprintReadWrite) uint32   Seq = 0;                      // ����ţ���ѡ��
};

class AMMOARPGMonster;
/**
 *  �������������������� AI��ֻ��������״̬ �� �ͻ���ƽ�����֡�
 *  ��Ҫ����Ҫ�� Monster �˵��� Patrol/OnAggroedPulled �� AI �ӿڣ���Щ���ڵ�����������
 */
UCLASS()
class MMOARPG_API AMMOARPGNetEnemyController : public AMMOARPGEnemyController
{
    GENERATED_BODY()


public:

	AMMOARPGNetEnemyController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // ========== �����ι���ݵĽӿڣ����ڽ��հ�ʱ���ã� ==========
    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void QueueNetState(const FNetMonsterState& InState);

    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void QueueNetStates(const TArray<FNetMonsterState>& States);

    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void ClearStateBuffer();

    // �������루����/��������룩
    UFUNCTION(BlueprintCallable, Category="MMOARPG|Enemy|Net")
    void SnapToState(const FNetMonsterState& InState, bool bAdjustZ = true);

    // ========== �ɵ����� ==========
    // ��ֵ�ӳ٣�Խ��Խ�ȡ�ԽСԽ���֣������� 0.08~0.2
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    float InterpDelaySeconds = 0.12f;

    // ƫ�����ֱ��˲�ƣ�cm��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    float SnapThreshold = 300.f;

    // Ӧ��λ��ʱ�Ƿ�����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MMOARPG|Enemy|Net")
    bool bAdjustZOnApply = true;

    // �� AnimBP ���ı���������󶨣�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MMOARPG|Enemy|Net")
    float NetSpeed = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MMOARPG|Enemy|Net")
    ENetMonsterAction NetAction = ENetMonsterAction::Idle;

public:
	// �����ƶ���ĳ���������ꡱ������״̬��ӣ��޷�����ʱ���ʱ�ñ���ʱ�䣩
	void Net_MoveTo(const FVector& Target, float Speed = 0.f, bool bChasing = false);

	// ������ 8400/���������õ��ˡ�������ʱ�������������汾��׼
	// Լ����ServerTimeSeconds �ǡ��롱Ϊ��λ���� StateBuffer �� ServerTime һ�£�
	void Net_MoveTo_At(const FVector& Target, double ServerTimeSeconds, float Speed = 0.f, bool bChasing = false);

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // === �ڲ���״̬���� & ��ֵ ===
    double ComputeClientRenderTime() const;
    void   InsertStateSorted(const FNetMonsterState& InState);
    bool   SampleStates(double ClientRenderTime, FNetMonsterState& OutPrev, FNetMonsterState& OutNext) const;
    void   ApplyInterpolated(AMMOARPGMonster* tEnemyPawn, const FNetMonsterState& A, const FNetMonsterState& B, double ClientRenderTime, float DeltaSeconds);

    // ���أ����ø��������߼�ǩ����ͬ������дһ����̬���ߣ�
    void   AdjustZToGround(AMMOARPGMonster* tEnemyPawn, FVector& InOutPos) const;

private:
    UPROPERTY() TArray<FNetMonsterState> StateBuffer;
    UPROPERTY(EditAnywhere, Category="MMOARPG|Enemy|Net") int32 MaxBufferSize = 64;

    FVector PrevVisualPos = FVector::ZeroVector;
    bool    bHasPrevVisualPos = false;
};
