
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterAnimTable.generated.h"

USTRUCT(BlueprintType)
struct FMonsterAnimRow : public FTableRowBase
{
    GENERATED_BODY()

    // 怪物ID（和服务器下发的 id 一致）
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MonsterId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	TSoftClassPtr<class AMMOARPGMonster> MonsterBlueprint;

	// 模型（SkeletalMesh）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
    TSoftObjectPtr<USkeletalMesh> Mesh;

    // 动画蓝图（动画控制器）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
    TSoftObjectPtr<UAnimBlueprint> AnimBlueprint;

    // 攻击动画（用软引用，避免强依赖与启动加载）
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSoftObjectPtr<UAnimMontage>> AttackMontages;

    // 可选：待机/受击/死亡/技能等
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Idle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Hit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Death;

    // 可选：模型/骨架等（有需要再放）
    // UPROPERTY(EditAnywhere, BlueprintReadOnly)
    // TSoftObjectPtr<USkeletalMesh> Mesh;
};