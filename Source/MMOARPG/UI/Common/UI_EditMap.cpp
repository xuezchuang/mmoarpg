// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_EditMap.h"
//#include "UserData.h"
#include "Components/EditableTextBox.h"
#include "GameFramework/PlayerStart.h"
//#include "WorldTools.h"
#include "Kismet/GameplayStatics.h"
//#include "BlueprintFunction.h"
#include "Components/Button.h"
#include "Chaos/Map.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "EngineUtils.h"
//#include "RobotBase.h"
#include "MMOARPG.h"
#include <corecrt_io.h>

#define	MAX_FILENAME_LEN  250
#define	C_WORLDMAP_ONE_GRID 100//单位 100 cm一个格子

UUI_EditMap::UUI_EditMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUI_EditMap::NativeConstruct()
{
	Super::NativeConstruct();
	button->OnClicked.AddDynamic(this, &UUI_EditMap::OnButtonClicked);
}

void UUI_EditMap::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_EditMap::Active()
{
	SetVisibility(ESlateVisibility::Visible);
	//file_input->forc
}

void UUI_EditMap::OnButtonClicked()
{
	UWorld* World = GetWorld();
	m_MapData.reset();
	FVector left, right;
	left = right = FVector::ZeroVector;
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("No Get World"));
		return;
	}
	int nUserBorder = 0;
	for (TActorIterator<AActor> It(World, LeftPos_BPClass); It; ++It)
	{
		left = It->GetActorLocation();
		left.X += 50.0f;
		left.Y -= 50.0f;
		nUserBorder++;
		break;
	}
	for (TActorIterator<AActor> It(World, RightPos_BPClass); It; ++It)
	{
		right = It->GetActorLocation();
		right.X += 50.0f;
		right.Y -= 50.0f;
		nUserBorder++;
		break;
	}
	if(nUserBorder != 2)
	{
#if WITH_EDITOR
		TArray<AActor*> LandscapeActors;
		UGameplayStatics::GetAllActorsOfClass(World, ALandscape::StaticClass(), LandscapeActors);
		if (LandscapeActors.IsEmpty())
		{
			UE_LOG(MMOARPG, Display, TEXT("no Find LandScape"));
			return;
		}
		const ALandscape* Landscape = Cast<ALandscape>(LandscapeActors[0]);
		if (Landscape == NULL)
		{
			UE_LOG(MMOARPG, Display, TEXT("no Find LandScape"));
			return;
		}
		FBox fbox = Landscape->GetCompleteBounds();
		UE_LOG(MMOARPG, Display, TEXT("leftPos:%.2f,%.2f,%.2f;RightPos:%.2f,%.2f,%.2f"), fbox.Min[0], fbox.Min[1], fbox.Min[2], fbox.Max[0], fbox.Max[1], fbox.Max[2]);

		left.X = fbox.Max[0]; left.Y = fbox.Min[1]; left.Z = fbox.Min[2];
		right.X = fbox.Min[0]; right.Y = fbox.Max[1]; right.Z = fbox.Max[2];
#endif
	}
	m_MapData.x = left.X;
	m_MapData.y = left.Y;
	m_MapData.z = left.Z;

	m_MapData.row = abs(right.X - left.X) / C_WORLDMAP_ONE_GRID;
	m_MapData.col = abs(right.Y - left.Y) / C_WORLDMAP_ONE_GRID;

	//
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStartActors);
	if (!PlayerStartActors.IsEmpty())
	{
		FVector pos = PlayerStartActors[0]->GetActorLocation();
		FS_GRID_BASE grid;
		posToGrid(&grid, &pos, &left);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("pos:%f,%f, born grid:%d,%d"), pos.X,pos.Y,grid.row, grid.col));
		insertBinValue(grid.row, grid.col, 2);

		FVector Newpos;
		gridToPos(&grid, &Newpos, &left);
		UE_LOG(MMOARPG, Display, TEXT("grid [%d,%d],value [%d],NewPos [%f,%f]"), grid.row, grid.col, m_MapData.value[grid.row][grid.col], Newpos.X, Newpos.Y);
	}
	//
	//TArray<AActor*> MonsterActors;
	//UGameplayStatics::GetAllActorsOfClass(World, ARobotBase::StaticClass(), MonsterActors);
	//if(!MonsterActors.IsEmpty())
	//{
	//	for(AActor* it : MonsterActors)
	//	{
	//		ARobotBase* monsterActor = Cast<ARobotBase>(it);
	//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("monste_id:%d"), monsterActor->data.id));
	//		FVector pos = monsterActor->GetActorLocation();
	//		FS_GRID_BASE grid;
	//		posToGrid(&grid, &pos, &left);
	//		////GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("pos:%f,%f, born grid:%d,%d"), pos.X,pos.Y,grid.row, grid.col));
	//		insertBinValue(grid.row, grid.col, monsterActor->data.id);
	//	}
	//}
	WriteMapFile();
}

void UUI_EditMap::WriteMapFile()
{
	FString filename = file_input->GetText().ToString();
	FString basedir = FPaths::ProjectDir();
	basedir = FPaths::Combine(*basedir, TEXT("Binaries/Win64/bin"));
	basedir = basedir.Replace(TEXT("/"), TEXT("//"));

	TCHAR* pSendData = basedir.GetCharArray().GetData();
	int nDataLen = FCString::Strlen(pSendData);
	char* binPath = (char*)TCHAR_TO_ANSI(pSendData);

	if(m_MapData.row >= MAX_ROW || m_MapData.row <= 0) return;
	if(m_MapData.col >= MAX_COL || m_MapData.col <= 0) return;

	int writeBytes = m_MapData.row * m_MapData.col;
	char* Data = new char[writeBytes];
	memset(Data, 0, writeBytes);

	for(int i = 0; i < m_MapData.row; i++)
	{
		for(int j = 0; j < m_MapData.col; j++)
		{
			uint8  v = m_MapData.value[i][j];
			int index = i * m_MapData.row + j;
			if(v != 0)
			{
				UE_LOG(MMOARPG, Display, TEXT("i:%d,j:%d,value:%d"), i, j, v);
			}
			Data[index] = v;
		}
	}

	//获取路径
	char fname[MAX_FILENAME_LEN];
	memset(fname, 0, MAX_FILENAME_LEN);
	sprintf_s(fname, "%s//%s.bin", binPath, TCHAR_TO_ANSI(*filename));

	//判断文件存在吗？
	int isexist = _access(binPath, 0);
	if(isexist == -1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("binPath Error"));
		UE_LOG(MMOARPG, Display, TEXT("binpath no exist:%s"), ANSI_TO_TCHAR(binPath));
		delete[] Data;
		return;
	}
	isexist = _access(fname, 0);
	//1 打开文件
	FILE* m_File = NULL;
	if(isexist == -1)
	{
		m_File = _fsopen(fname, "wb+", _SH_DENYNO);
		if(m_File == NULL) return;
		fseek(m_File, 0, SEEK_SET);
		bool  iswrite = true;//是否可以写？
		fwrite(&iswrite, 1, sizeof(bool), m_File);
		fclose(m_File);
	}

	//2 写20个字节到文件中
	m_File = _fsopen(fname, "rb+", _SH_DENYNO);
	fseek(m_File, 0, SEEK_SET);
	size_t ret_size = fwrite(&m_MapData, 1, 20, m_File);

	//3 指针偏移到索引20的位置开始写文件
	fseek(m_File, 20, SEEK_SET);
	fwrite(Data, 1, writeBytes, m_File);
	fclose(m_File);

	//最后删除
	delete[] Data;
}

void UUI_EditMap::posToGrid(FS_GRID_BASE* grid, FVector* pos, FVector* left)
{
	grid->row = (abs(pos->X - left->X) / (C_WORLDMAP_ONE_GRID));
	grid->col = (abs(pos->Y - left->Y) / (C_WORLDMAP_ONE_GRID));
}
//网格坐标转为3D坐标
void UUI_EditMap::gridToPos(FS_GRID_BASE* grid, FVector* pos, FVector* left)
{
	pos->X = left->X - (grid->row * C_WORLDMAP_ONE_GRID + 50);
	pos->Y = left->Y + grid->col * C_WORLDMAP_ONE_GRID + 50;
}

void UUI_EditMap::insertBinValue(int row, int col, int id)
{
	if(row >= MAX_ROW || row < 0) return;
	if(col >= MAX_COL || col < 0) return;

	m_MapData.value[row][col] = id;
}