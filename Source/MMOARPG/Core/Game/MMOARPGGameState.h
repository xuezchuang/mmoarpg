// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../../DataTable/CharacterAnimTable.h"
#include "../../DataTable/CharacterStyleTable.h"
#include "MMOARPGGameState.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMMOARPGGameState();

	FCharacterAnimTable* GetCharacterAnimTable(int32 InAnimTableID);
	TArray<FCharacterAnimTable*>* GetCharacterAnimTables();

	FCharacterStyleTable* GetCharacterStyleTable(int32 InCharacterTableID);
	TArray<FCharacterStyleTable*>* GetCharacterStyleTables();
protected:
	template<class T>
	TArray<T*>* GetTables(UDataTable* InTable, TArray<T*>& OutTables, const FString& MsgTag = TEXT("MyTable"))
	{
		if (!OutTables.Num())
		{
			if (InTable)
			{
				InTable->GetAllRows(MsgTag, OutTables);
			}
		}

		return &OutTables;
	}

	template<class T>
	T* GetTable(int32 InTableID, UDataTable* InTable, TArray<T*>& OutTables, const FString& MsgTag = TEXT("MyTable"))
	{
		if (TArray<T*>* InTables = GetTables<T>(InTable, OutTables, MsgTag))
		{
			if (InTables->Num())
			{
				if (auto ItemInTable = InTables->FindByPredicate([&](T* InMyTable) {return InMyTable->ID == InTableID;}))
				{
					return *ItemInTable;
				}
			}
		}

		return NULL;
	}

protected:
	UPROPERTY()
	UDataTable* CharacterAnimTablePtr;

	UPROPERTY()
	UDataTable* CharacterStyleTablePtr;

	TArray<FCharacterAnimTable*> CharacterAnimTables;
	TArray<FCharacterStyleTable*> CharacterStyleTables;
};
