#pragma once
#include "CoreMinimal.h"
#include "MMOARPTool.generated.h"

#define	MAX_FILENAME_LEN  250
#define	C_WORLDMAP_ONE_GRID 100//单位 100 cm一个格子

USTRUCT(BlueprintType)
struct FS_GRID_BASE
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 row = 0; // 0-based
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 col = 0; // 0-based
};

USTRUCT(BlueprintType)
struct FWorldMapInfo
{
    GENERATED_BODY()
    // 左上角（Origin = LeftTop）。若你“暂时把 left 当 0”，就给 Origin = (0,0,0) 即可
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Origin = FVector::ZeroVector;
    // 单格尺寸（厘米）
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float GridSize = 100.f;
    // 可选：行列范围（用于 Clamp）
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Rows = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Cols = 0;
};

UCLASS()
class MMOARPG_API UMMOARPTool : public UObject
{
    GENERATED_BODY()
public:
    // 世界坐标 -> 网格（0 起始）。默认会 Clamp 到合法范围（若提供了 Rows/Cols）
    UFUNCTION(BlueprintCallable, Category="MMOARP|Grid")
    static void PosToGrid(FS_GRID_BASE& OutGrid, const FVector& Pos, const FWorldMapInfo& Map, bool bClampToRange = true);

    // 网格 -> 世界坐标。bUseCellCenter=true 表示落在格子中心；false 落在格子左上角
    UFUNCTION(BlueprintCallable, Category="MMOARP|Grid")
    static void GridToPos(const FS_GRID_BASE& Grid, FVector& OutPos, const FWorldMapInfo& Map, bool bUseCellCenter = true);

    // 简化重载：只给 Origin/Size
    static FORCEINLINE FS_GRID_BASE PosToGridSimple(const FVector& Pos, const FVector& Origin, float GridSize)
    {
        FWorldMapInfo Map; Map.Origin = Origin; Map.GridSize = GridSize;
        FS_GRID_BASE G; PosToGrid(G, Pos, Map, /*Clamp*/false); return G;
    }

    static FORCEINLINE FVector GridToPosSimple(const FS_GRID_BASE& Grid, const FVector& Origin, float GridSize, bool bUseCellCenter = true)
    {
        FWorldMapInfo Map; Map.Origin = Origin; Map.GridSize = GridSize;
        FVector P; GridToPos(Grid, P, Map, bUseCellCenter); return P;
    }
};
