// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGGameState.h"

AMMOARPGGameState::AMMOARPGGameState()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterAnimTable(TEXT("/Game/DataTable/CharacterAnimTable"));
	CharacterAnimTablePtr = CharacterAnimTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterStyleTable(TEXT("/Game/DataTable/CharacterStyleTable"));
	CharacterStyleTablePtr = CharacterStyleTable.Object;
}

FCharacterAnimTable* AMMOARPGGameState::GetCharacterAnimTable(int32 InAnimTableID)
{
	return GetTable(InAnimTableID, CharacterAnimTablePtr, CharacterAnimTables, TEXT("AnimTable"));
}

TArray<FCharacterAnimTable*>* AMMOARPGGameState::GetCharacterAnimTables()
{
	return GetTables(CharacterAnimTablePtr, CharacterAnimTables,TEXT("AnimTable"));
}

FCharacterStyleTable* AMMOARPGGameState::GetCharacterStyleTable(int32 InCharacterTableID)
{
	return GetTable(InCharacterTableID, CharacterStyleTablePtr, CharacterStyleTables, TEXT("CharacterTable"));
}

TArray<FCharacterStyleTable*>* AMMOARPGGameState::GetCharacterStyleTables()
{
	return GetTables(CharacterStyleTablePtr, CharacterStyleTables, TEXT("CharacterTable"));
}
