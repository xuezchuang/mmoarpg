// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_MainBase.h"
#include "UI_HallMain.generated.h"

class UUI_CharacterCreatePanel;
class UUI_RenameCreate;
class UUI_EditorCharacter;
/**
 * 
 */
UCLASS()
class MMOARPG_API UUI_HallMain : public UUI_MainBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUI_CharacterCreatePanel* UI_CharacterCreatePanel;

	UPROPERTY(meta = (BindWidget))
	UUI_RenameCreate* UI_RenameCreate;

	UPROPERTY(meta = (BindWidget))
	UUI_EditorCharacter* UI_EditorCharacter;

	UPROPERTY(meta = (BindWidget))
	UUI_Print* UI_Print;
public:
	enum ECAType
	{
		CA_EDITOR,
		CA_CREATE,
	};

public:
	UUI_HallMain(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);
protected:
	virtual void LinkInit() override;
public:
	void ResetEidtorType();
	void ResetCreateType();

	void PlayRenameIn();
	void PlayRenameOut();

	void PlayEditorCharacterIn();
	void PlayEditorCharacterOut();

	void ResetCharacterCreatePanel(bool bSpawnNewCharacter = true);
	void ResetCharacterAppearance(FMMOARPGCharacterAppearance* InCA);
	void SpawnRecentCharacter();
	void HighlightDefaultSelection();
	void CheckRename(FString &InCharacterName);
	void CreateCharacter(const FMMOARPGCharacterAppearance &InCA);
	void SetSlotPosition(const int32 InSlotPosition);

	void JumpDSServer(int32 InSlotID);

	void LoginCharacter(int32 InSlot);
	void EditCharacter(int32 InSlot);
	void DeleteCharacter(int32 InSlot);
	void SetEditCharacter(const FMMOARPGCharacterAppearance* InCA);

	void DestroyCharacter();

	
protected:
	void HallMainIn();
	void HallMainOut();
protected:

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	//virtual void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

	void PrintLogByCheckName(ECheckNameType InCheckNameType);
private:
	FMMOARPGCharacterAppearance StartACData;
	ECAType CAType;
};
