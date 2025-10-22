
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterAnimTable.h"

namespace MonsterAnimLib
{
    // 根据怪物ID查 DataTable 行
    inline const FMonsterAnimRow* FindRowById(const UDataTable* Table, int32 MonsterId)
    {
        if (!Table) return nullptr;
        static const FString Ctx(TEXT("MonsterAnimLookup"));
        // 建议把 RowName 也设置为 MonsterId 的字符串（如 "10001"），这样可直接 FindRow
        const FName RowName = FName(*FString::FromInt(MonsterId));
        if (const FMonsterAnimRow* Row = Table->FindRow<FMonsterAnimRow>(RowName, Ctx))
        {
            return Row;
        }
        // 备选：遍历（如果 RowName 不是 id）
        for (const auto& It : Table->GetRowMap())
        {
            const FMonsterAnimRow* Row = (const FMonsterAnimRow*)It.Value;
            if (Row && Row->MonsterId == MonsterId) return Row;
        }
        return nullptr;
    }

    // 播放一段攻击动画（随机挑一个），自动异步加载
    inline void PlayAttackMontage(ACharacter* Char, const UDataTable* Table, int32 MonsterId)
    {
        if (!Char) return;
        const auto* Row = FindRowById(Table, MonsterId);
        if (!Row || Row->AttackMontages.Num() == 0) return;

        const int32 Index = FMath::RandRange(0, Row->AttackMontages.Num() - 1);
        TSoftObjectPtr<UAnimMontage> Soft = Row->AttackMontages[Index];

        if (UAnimMontage* Montage = Soft.Get())  // 已加载
        {
            Char->PlayAnimMontage(Montage);
            return;
        }

        FStreamableManager& SM = UAssetManager::GetStreamableManager();
        const FSoftObjectPath Path = Soft.ToSoftObjectPath();
        SM.RequestAsyncLoad(Path, [Char, Soft]()
        {
            if (UAnimMontage* M = Soft.Get())
            {
                Char->PlayAnimMontage(M);
            }
        });
    }
}