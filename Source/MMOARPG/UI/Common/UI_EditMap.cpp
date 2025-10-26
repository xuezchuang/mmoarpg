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
#include "MMOARPGMonster.h"
#include "MMOARPTool.h"



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
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        // ��ʾ���
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;

        // �л�Ϊ���� UI ģʽ��
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());  // �õ�ǰUI��ý���
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
    }
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

	//m_MapData.row = abs(right.X - left.X) / C_WORLDMAP_ONE_GRID;
	//m_MapData.col = abs(right.Y - left.Y) / C_WORLDMAP_ONE_GRID;
	m_MapData.row = FMath::FloorToInt( FMath::Abs(right.X - left.X) / C_WORLDMAP_ONE_GRID );
	m_MapData.col = FMath::FloorToInt(FMath::Abs(right.Y - left.Y) / C_WORLDMAP_ONE_GRID);

	//
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStartActors);
	if (!PlayerStartActors.IsEmpty())
	{
		FVector pos = PlayerStartActors[0]->GetActorLocation();
		FS_GRID_BASE grid;
		//posToGrid(&grid, &pos, &left);
		FWorldMapInfo Map;
		Map.Origin = left;                // ���Ͻ�Ϊԭ��
		Map.GridSize = C_WORLDMAP_ONE_GRID;
		UMMOARPTool::PosToGrid(grid, pos, Map);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("pos:%f,%f, born grid:%d,%d"), pos.X,pos.Y,grid.row, grid.col));
		insertBinValue(grid.row, grid.col, 2);

		FVector Newpos;
		UMMOARPTool::GridToPos(grid, Newpos, Map, true);
		UE_LOG(MMOARPG, Display, TEXT("grid [%d,%d],value [%d],NewPos [%f,%f]"), grid.row, grid.col, m_MapData.value[grid.row][grid.col], Newpos.X, Newpos.Y);
	}
	
	PendingMapObjects.Reset();

	TArray<AActor*> MonsterActors;
	UGameplayStatics::GetAllActorsOfClass(World, AMMOARPGMonster::StaticClass(), MonsterActors);
	if (!MonsterActors.IsEmpty())
	{
		for (AActor* it : MonsterActors)
		{
			AMMOARPGMonster* monsterActor = Cast<AMMOARPGMonster>(it);
			if (!monsterActor) continue;

			const int32 monsterId = monsterActor->MonsterID;
			FVector pos = monsterActor->GetActorLocation();

			FS_GRID_BASE grid;
			FWorldMapInfo Map; Map.Origin = left; Map.GridSize = C_WORLDMAP_ONE_GRID;
			UMMOARPTool::PosToGrid(grid, pos, Map);

			// ����ѡ�����ٰ� ID ���� m_MapData.value ���������豣��ռλ����дһ���̶�������
			// insertBinValue(grid.row, grid.col, EGTY_MONSTER_SPAWN); // ��������������ö��
			// ��֮ǰ�� insertBinValue(grid.row, grid.col, monsterId) �����Ƴ�����������ID��Ⱦ

			// �ռ�д��β�����Ŀ
			S_MAPOBJ_ENTRY e{};
			e.row = (uint16)FMath::Clamp(grid.row, 0, m_MapData.row - 1);
			e.col = (uint16)FMath::Clamp(grid.col, 0, m_MapData.col - 1);
			e.id = (uint32)monsterId;
			e.kind = EMO_MONSTER;

			PendingMapObjects.Add(e);

			UE_LOG(MMOARPG, Display, TEXT("Monster Pos[%d,%d], id:%d"),(int)e.row, (int)e.col, monsterId);
		}
	}
	WriteMapFile();

}

void UUI_EditMap::WriteMapFile()
{
	FString filename = file_input->GetText().ToString();
	FString basedir = FPaths::ProjectDir();
	basedir = FPaths::Combine(*basedir, TEXT("Binaries/bin"));
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

	//��ȡ·��
	char fname[MAX_FILENAME_LEN];
	memset(fname, 0, MAX_FILENAME_LEN);
	sprintf_s(fname, "%s//%s.bin", binPath, TCHAR_TO_ANSI(*filename));

	//�ж��ļ�������
	int isexist = _access(binPath, 0);
	if(isexist == -1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("binPath Error"));
		UE_LOG(MMOARPG, Display, TEXT("binpath no exist:%s"), ANSI_TO_TCHAR(binPath));
		delete[] Data;
		return;
	}
	isexist = _access(fname, 0);
	//1 ���ļ�
	FILE* m_File = NULL;
	if(isexist == -1)
	{
		m_File = _fsopen(fname, "wb+", _SH_DENYNO);
		if(m_File == NULL) return;
		fseek(m_File, 0, SEEK_SET);
		bool  iswrite = true;//�Ƿ����д��
		fwrite(&iswrite, 1, sizeof(bool), m_File);
		fclose(m_File);
	}

	//2 д20���ֽڵ��ļ���
	m_File = _fsopen(fname, "rb+", _SH_DENYNO);
	fseek(m_File, 0, SEEK_SET);
	size_t ret_size = fwrite(&m_MapData, 1, 20, m_File);

	//3 ָ��ƫ�Ƶ�����20��λ�ÿ�ʼд�ļ�
	fseek(m_File, 20, SEEK_SET);
	fwrite(Data, 1, writeBytes, m_File);

	// ===== ׷�Ӷ���飨����ȣ����ļ�β =====
	fseek(m_File, 0, SEEK_END);

	// ֻ������Ŀʱд��飻û��Ŀ�Ͳ�д�������ļ���С�Ҽ���
	if (PendingMapObjects.Num() > 0)
	{
		S_MAPOBJ_BLOCK_HDR hdr{};
		hdr.magic = MAPOBJ_MAGIC;
		hdr.version = MAPOBJ_VERSION;
		hdr.entry_size = MAPOBJ_ENTRY_SIZE;
		hdr.count = (uint32_t)PendingMapObjects.Num();

		// д��ͷ
		fwrite(&hdr, 1, sizeof(hdr), m_File);

		// д N �� Entry���ڴ�������һ����д����
		fwrite(PendingMapObjects.GetData(),
			sizeof(S_MAPOBJ_ENTRY),
			PendingMapObjects.Num(),
			m_File);

		// ��ѡ��д 4 �ֽ�У�飨����д 0��
		uint32_t checksum = 0;
		fwrite(&checksum, 1, sizeof(uint32_t), m_File);
	}

	fclose(m_File);
	//���ɾ��
	delete[] Data;
}

//void UUI_EditMap::posToGrid(FS_GRID_BASE* grid, FVector* pos, FVector* left)
//{
//	grid->row = (abs(pos->X - left->X) / (C_WORLDMAP_ONE_GRID));
//	grid->col = (abs(pos->Y - left->Y) / (C_WORLDMAP_ONE_GRID));
//}
////��������תΪ3D����
//void UUI_EditMap::gridToPos(FS_GRID_BASE* grid, FVector* pos, FVector* left)
//{
//	pos->X = left->X - (grid->row * C_WORLDMAP_ONE_GRID + 50);
//	pos->Y = left->Y + grid->col * C_WORLDMAP_ONE_GRID + 50;
//}

void UUI_EditMap::insertBinValue(int row, int col, int id)
{
	if(row >= MAX_ROW || row < 0) return;
	if(col >= MAX_COL || col < 0) return;

	m_MapData.value[row][col] = id;
}