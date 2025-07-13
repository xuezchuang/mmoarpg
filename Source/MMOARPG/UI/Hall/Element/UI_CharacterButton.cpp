// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_CharacterButton.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "../../../Core/Hall/Character/CharacterStage.h"
#include "../../../Core/Hall/HallPawn.h"
#include "UI_CharacterCreatePanel.h"
#include "../UI_HallMain.h"
#include "../../../Core/Hall/HallPlayerState.h"

void UUI_CharacterButton::NativeConstruct()
{
	Super::NativeConstruct();

	SlotPosition = INDEX_NONE;

	CharacterButton->OnReleased.AddDynamic(this, &UUI_CharacterButton::ClickedCharacter);

	CreateShowIcon->SetVisibility(ESlateVisibility::Visible);
	CharacterInfo->SetVisibility(ESlateVisibility::Collapsed);

	//DefaultColor = CharacterButton->WidgetStyle.Normal.TintColor.GetSpecifiedColor();
	DefaultColor = CharacterButton->GetStyle().Normal.TintColor.GetSpecifiedColor();
}

void UUI_CharacterButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CharacterButton::SetSlotPosition(const int32 InNewPos)
{
	SlotPosition = InNewPos;
}

void UUI_CharacterButton::InitCharacterButton(const FMMOARPGCharacterAppearance& InCA)
{
	if (InCA.SlotPosition != INDEX_NONE)
	{
		CreateShowIcon->SetVisibility(ESlateVisibility::Collapsed);
		CharacterInfo->SetVisibility(ESlateVisibility::Visible);

		CharacterName->SetText(FText::FromString(InCA.Name));
		Date->SetText(FText::FromString(InCA.Date));
	}
}

void UUI_CharacterButton::SetHighlight(bool bHighlight)
{
	FButtonStyle style = CharacterButton->GetStyle();
	if (bHighlight)
	{
		style.Normal.TintColor = HighlightColor;
	}
	else
	{
		style.Normal.TintColor = DefaultColor;
	}
	CharacterButton->SetStyle(style);
	
}

bool UUI_CharacterButton::IsHighlight()
{
	return CharacterButton->GetStyle().Normal.TintColor == HighlightColor;
}

void UUI_CharacterButton::ClickedCharacter()
{
	if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
	{
		if (UUI_CharacterCreatePanel* UI_CharacterCreatePanel = GetParents<UUI_CharacterCreatePanel>())
		{
			//������ɫ
			if (!InState->IsCharacterExistInSlot(SlotPosition))
			{
				if (ACharacterStage *InCS = UI_CharacterCreatePanel->CreateCharacter())
				{
					if (InState->GetCurrentTmpCreateCharacter())
					{
						InState->GetCurrentTmpCreateCharacter()->Reset();
						InState->GetCurrentTmpCreateCharacter()->SlotPosition = SlotPosition;
					}

					InCS->SetSlotID(SlotPosition);
					InCS->UpdateKneadingBoby();
				}	

				UI_CharacterCreatePanel->CreateKneadFace();
				if (UUI_HallMain* UI_HallMain = UI_CharacterCreatePanel->GetParents<UUI_HallMain>())
				{
					UI_HallMain->PlayRenameIn();
					UI_HallMain->SetSlotPosition(SlotPosition);

					UI_HallMain->ResetCreateType();
					UI_HallMain->PlayEditorCharacterOut();
				}
			}
			//���Ѿ����ڵĽ�ɫ�����ͼ
			else if(UI_CharacterCreatePanel->GetHighlightButton() == this)
			{
				//DS������
				JumpDSServer();
			}
			//���Լ�����Ϊ����
			else
			{
				UI_CharacterCreatePanel->GetHighlightButton()->SetHighlight(false);
				SetHighlight(true);

				if (FMMOARPGCharacterAppearance *InCA = InState->GetCharacterCA(SlotPosition))
				{
					UI_CharacterCreatePanel->SpawnCharacter(InCA);
					UI_CharacterCreatePanel->SetEditCharacter(InCA);
				}
				else
				{
					UI_CharacterCreatePanel->SetEditCharacter(NULL);
				}
			}

			UI_CharacterCreatePanel->SetCurrentSlotPosition(SlotPosition);
		}
	}
}

void UUI_CharacterButton::JumpDSServer()
{
	if (UUI_CharacterCreatePanel* InUI_CharacterCreatePanel = GetParents<UUI_CharacterCreatePanel>())
	{
		InUI_CharacterCreatePanel->JumpDSServer(SlotPosition);
	}
}
