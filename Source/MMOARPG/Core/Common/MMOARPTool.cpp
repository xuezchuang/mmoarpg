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

// 约定回顾（以 PosToGrid 为准）
// row = floor( (Origin.X - Pos.X) / S )    // 行沿 -X 递增
// col = floor( (Pos.Y   - Origin.Y) / S )  // 列沿 +Y 递增
// 同格的 X ∈ (Origin.X - (row+1)S, Origin.X - row*S] （半开区间）
// 同格的 Y ∈ [Origin.Y + col*S, Origin.Y + (col+1)S)

static FORCEINLINE FVector GridCellCenter(const FS_GRID_BASE& G, const FWorldMapInfo& Map, float Z = 0.f)
{
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;
    // 取格心，保证 PosToGrid(格心) == (row,col)
    const float x = Map.Origin.X - ( (static_cast<float>(G.row) + 0.5f) * S );
    const float y = Map.Origin.Y + ( (static_cast<float>(G.col) + 0.5f) * S );
    return FVector(x, y, Z);
}

static FORCEINLINE void GridCellAABB(const FS_GRID_BASE& G, const FWorldMapInfo& Map,
                                     FVector2f& OutMin, FVector2f& OutMax)
{
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;

    // X：向 -X 增行 → 这一行在 [Origin.X - (row+1)S, Origin.X - row*S]
    const float xMin = Map.Origin.X - ( (static_cast<float>(G.row) + 1.0f) * S );
    const float xMax = Map.Origin.X - ( (static_cast<float>(G.row) + 0.0f) * S );

    // Y：向 +Y 增列 → 这一列在 [Origin.Y + col*S, Origin.Y + (col+1)S]
    const float yMin = Map.Origin.Y + ( (static_cast<float>(G.col) + 0.0f) * S );
    const float yMax = Map.Origin.Y + ( (static_cast<float>(G.col) + 1.0f) * S );

    OutMin = FVector2f(xMin, yMin);
    OutMax = FVector2f(xMax, yMax);
}

void UMMOARPTool::GridToPos(const FS_GRID_BASE& Grid, FVector& OutPos,
                            const FWorldMapInfo& Map, bool bUseCellCenter)
{
    if (bUseCellCenter)
    {
        // 用格心：可确保 PosToGrid(OutPos) == Grid
        const FVector Center = GridCellCenter(Grid, Map, OutPos.Z);
        OutPos.X = Center.X;
        OutPos.Y = Center.Y;
        // Z 留给外部贴地/射线
        return;
    }

    // 若不取格心：默认给“格内稳定代表点”（靠近左下角稍偏内，避免落到边界导致 floor 抖动）
    const float S = (Map.GridSize > 0.f) ? Map.GridSize : 100.f;
    constexpr float EPS = 1e-3f;             // 比 S 小很多的正数
    const float inset = EPS * S;             // 向格内偏移一点点

    FVector2f bmin, bmax;
    GridCellAABB(Grid, Map, bmin, bmax);

    // 取到“格内”的一个确定点（X 取上边界往内推一点；Y 取下边界往内推一点）
    OutPos.X = static_cast<double>(bmax.X) - inset;   // 仍在该格内
    OutPos.Y = static_cast<double>(bmin.Y) + inset;   // 仍在该格内
    // Z 仍交给外部
}

