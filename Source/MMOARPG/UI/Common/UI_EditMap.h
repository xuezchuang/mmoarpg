#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_EditMap.generated.h"

class UButton;
class UEditableTextBox;
//class UUserWidget;

#define MAX_ROW 1000
#define MAX_COL 1000

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

};
