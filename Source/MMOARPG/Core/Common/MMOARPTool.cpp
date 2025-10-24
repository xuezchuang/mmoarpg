#include "MMOARPTool.h"

static FORCEINLINE int32 ClampIndex(int32 V, int32 MaxExclusive)
{
    if (MaxExclusive <= 0) return V; // δ���÷�Χ������
    return FMath::Clamp(V, 0, MaxExclusive - 1);
}

void UMMOARPTool::PosToGrid(FS_GRID_BASE& OutGrid, const FVector& Pos, const FWorldMapInfo& Map, bool bClampToRange)
{
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;

    // ע���� FMath::*����Ҫ�� <cmath> �� abs/round��������������
    const float dx = (Map.Origin.X - Pos.X);   // �� -X ����Ϊ��
    const float dy = (Pos.Y - Map.Origin.Y);   // �� +Y ����Ϊ��

    int32 row = FMath::FloorToInt(dx / S);
    int32 col = FMath::FloorToInt(dy / S);

    if (bClampToRange)
    {
        if (Map.Rows > 0) row = ClampIndex(row, Map.Rows);
        if (Map.Cols > 0) col = ClampIndex(col, Map.Cols);
    }

    OutGrid.row = row;
    OutGrid.col = col;
}

void UMMOARPTool::GridToPos(const FS_GRID_BASE& Grid, FVector& OutPos, const FWorldMapInfo& Map, bool bUseCellCenter)
{
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;
    const float Offset = bUseCellCenter ? 0.5f : 0.f;

    // ������ PosToGrid ��ȫ���棨���� -X������ +Y��
    OutPos.X = Map.Origin.X - ( (static_cast<float>(Grid.row) + Offset) * S );
    OutPos.Y = Map.Origin.Y + ( (static_cast<float>(Grid.col) + Offset) * S );
    // Z ���������߼���AdjustZToGround �����ͳһȡ�ߺ�����
    // OutPos.Z �����ⲿ��ֵ/���߽��
}
