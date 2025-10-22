
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterAnimTable.h"

namespace MonsterAnimLib
{
    // ���ݹ���ID�� DataTable ��
    inline const FMonsterAnimRow* FindRowById(const UDataTable* Table, int32 MonsterId)
    {
        if (!Table) return nullptr;
        static const FString Ctx(TEXT("MonsterAnimLookup"));
        // ����� RowName Ҳ����Ϊ MonsterId ���ַ������� "10001"����������ֱ�� FindRow
        const FName RowName = FName(*FString::FromInt(MonsterId));
        if (const FMonsterAnimRow* Row = Table->FindRow<FMonsterAnimRow>(RowName, Ctx))
        {
            return Row;
        }
        // ��ѡ����������� RowName ���� id��
        for (const auto& It : Table->GetRowMap())
        {
            const FMonsterAnimRow* Row = (const FMonsterAnimRow*)It.Value;
            if (Row && Row->MonsterId == MonsterId) return Row;
        }
        return nullptr;
    }

    // ����һ�ι��������������һ�������Զ��첽����
    inline void PlayAttackMontage(ACharacter* Char, const UDataTable* Table, int32 MonsterId)
    {
        if (!Char) return;
        const auto* Row = FindRowById(Table, MonsterId);
        if (!Row || Row->AttackMontages.Num() == 0) return;

        const int32 Index = FMath::RandRange(0, Row->AttackMontages.Num() - 1);
        TSoftObjectPtr<UAnimMontage> Soft = Row->AttackMontages[Index];

        if (UAnimMontage* Montage = Soft.Get())  // �Ѽ���
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