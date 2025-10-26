#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_EditMap.generated.h"

class UButton;
class UEditableTextBox;
//class UUserWidget;

#define MAX_ROW 1000
#define MAX_COL 1000

#pragma pack(push, 1)

struct S_MAP_BIN_BASE
{
	int32  x;
	int32  y;
	int32  z;
	int row;
	int col;
	uint8 value[MAX_ROW][MAX_COL];

	inline void reset()
	{
		memset(value, 0, MAX_ROW * MAX_COL);
	}
};

enum : uint8_t
{
	EMO_MONSTER = 1,  // 怪物
	EMO_ORE = 2,  // 矿石/采集点
	EMO_HERB = 3,  // 草药
	EMO_CHEST = 4,  // 宝箱
	EMO_NPC = 5,  // NPC（如果未来需要）
	// ... 以后可继续加
};

// 头部（放在对象块最前面）
struct S_MAPOBJ_BLOCK_HDR
{
	uint32_t magic;      // 固定写 'MOBJ' = 0x4D4F424A
	uint16_t version;    // = 1
	uint16_t entry_size; // = sizeof(S_MAPOBJ_ENTRY)（自描述，便于扩展）
	uint32_t count;      // 条目数量 N
};

// 条目（仅坐标 + id + 类型）。坐标用 u16 足够覆盖你的行列范围。
struct S_MAPOBJ_ENTRY
{
	uint16_t row;    // 网格行
	uint16_t col;    // 网格列
	uint32_t id;     // 真实对象ID（怪物ID/矿石ID/…），由你的脚本/数据库解析
	uint8_t  kind;   // 对象类型：EMO_MONSTER / EMO_ORE / ...
	uint8_t  _pad[3];// 预留对齐/将来扩展（保持 entry_size 不变）
};

#pragma pack(pop)

static constexpr uint32_t MAPOBJ_MAGIC = ('M') | ('O' << 8) | ('B' << 16) | ('J' << 24); // "MOBJ"
static constexpr uint16_t MAPOBJ_VERSION = 1;
static constexpr uint16_t MAPOBJ_ENTRY_SIZE = sizeof(S_MAPOBJ_ENTRY);

UCLASS(Blueprintable, Abstract)
class UUI_EditMap: public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<AActor> LeftPos_BPClass;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<AActor> RightPos_BPClass;

	UPROPERTY(meta = (BindWidget))
	UButton* button;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* file_input;
public:
	UUI_EditMap(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void Active();
private:
	UFUNCTION()
	void OnButtonClicked();

	//坐标转为格子坐标 大-小
	//void posToGrid(FS_GRID_BASE* grid, FVector* pos, FVector* left);
	//网格坐标转为3D坐标
	//inline void gridToPos(FS_GRID_BASE* grid, FVector* pos, FVector* left);
	void insertBinValue(int row, int col, int id);

	void WriteMapFile();
private:
	S_MAP_BIN_BASE m_MapData;
	TArray<S_MAPOBJ_ENTRY> PendingMapObjects; 
};
