#include "MMOARPTool.h"

static FORCEINLINE int32 ClampIndex(int32 V, int32 MaxExclusive)
{
    if (MaxExclusive <= 0) return V; // 未设置范围，不夹
    return FMath::Clamp(V, 0, MaxExclusive - 1);
}

void UMMOARPTool::PosToGrid(FS_GRID_BASE& OutGrid, const FVector& Pos, const FWorldMapInfo& Map, bool bClampToRange)
{
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;

    // 注意用 FMath::*，不要用 <cmath> 的 abs/round，避免类型隐患
    const float dx = (Map.Origin.X - Pos.X);   // 向 -X 方向为正
    const float dy = (Pos.Y - Map.Origin.Y);   // 向 +Y 方向为正

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

    // 和上面 PosToGrid 完全互逆（行沿 -X，列沿 +Y）
    OutPos.X = Map.Origin.X - ( (static_cast<float>(Grid.row) + Offset) * S );
    OutPos.Y = Map.Origin.Y + ( (static_cast<float>(Grid.col) + Offset) * S );
    // Z 交给贴地逻辑（AdjustZToGround 或你的统一取高函数）
    // OutPos.Z 保持外部赋值/射线结果
}
