#pragma once
#include "CoreMinimal.h"
#include "MMOARPTool.generated.h"

#define	MAX_FILENAME_LEN  250
#define	C_WORLDMAP_ONE_GRID 100//��λ 100 cmһ������

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
    // ���Ͻǣ�Origin = LeftTop�������㡰��ʱ�� left �� 0�����͸� Origin = (0,0,0) ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Origin = FVector::ZeroVector;
    // ����ߴ磨���ף�
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float GridSize = 100.f;
    // ��ѡ�����з�Χ������ Clamp��
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Rows = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Cols = 0;
};

UCLASS()
class MMOARPG_API UMMOARPTool : public UObject
{
    GENERATED_BODY()
public:
    // �������� -> ����0 ��ʼ����Ĭ�ϻ� Clamp ���Ϸ���Χ�����ṩ�� Rows/Cols��
    UFUNCTION(BlueprintCallable, Category="MMOARP|Grid")
    static void PosToGrid(FS_GRID_BASE& OutGrid, const FVector& Pos, const FWorldMapInfo& Map, bool bClampToRange = true);

    // ���� -> �������ꡣbUseCellCenter=true ��ʾ���ڸ������ģ�false ���ڸ������Ͻ�
    UFUNCTION(BlueprintCallable, Category="MMOARP|Grid")
    static void GridToPos(const FS_GRID_BASE& Grid, FVector& OutPos, const FWorldMapInfo& Map, bool bUseCellCenter = true);

    // �����أ�ֻ�� Origin/Size
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
