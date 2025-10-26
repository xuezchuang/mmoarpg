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
	EMO_MONSTER = 1,  // ����
	EMO_ORE = 2,  // ��ʯ/�ɼ���
	EMO_HERB = 3,  // ��ҩ
	EMO_CHEST = 4,  // ����
	EMO_NPC = 5,  // NPC�����δ����Ҫ��
	// ... �Ժ�ɼ�����
};

// ͷ�������ڶ������ǰ�棩
struct S_MAPOBJ_BLOCK_HDR
{
	uint32_t magic;      // �̶�д 'MOBJ' = 0x4D4F424A
	uint16_t version;    // = 1
	uint16_t entry_size; // = sizeof(S_MAPOBJ_ENTRY)����������������չ��
	uint32_t count;      // ��Ŀ���� N
};

// ��Ŀ�������� + id + ���ͣ��������� u16 �㹻����������з�Χ��
struct S_MAPOBJ_ENTRY
{
	uint16_t row;    // ������
	uint16_t col;    // ������
	uint32_t id;     // ��ʵ����ID������ID/��ʯID/����������Ľű�/���ݿ����
	uint8_t  kind;   // �������ͣ�EMO_MONSTER / EMO_ORE / ...
	uint8_t  _pad[3];// Ԥ������/������չ������ entry_size ���䣩
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

	//����תΪ�������� ��-С
	//void posToGrid(FS_GRID_BASE* grid, FVector* pos, FVector* left);
	//��������תΪ3D����
	//inline void gridToPos(FS_GRID_BASE* grid, FVector* pos, FVector* left);
	void insertBinValue(int row, int col, int id);

	void WriteMapFile();
private:
	S_MAP_BIN_BASE m_MapData;
	TArray<S_MAPOBJ_ENTRY> PendingMapObjects; 
};
