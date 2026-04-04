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
	UE_LOG(MMOARPG, Display, TEXT("ClickedCharacter [Slot:%d]"), SlotPosition);

	if (AHallPlayerState* InState = GetPlayerState<AHallPlayerState>())
	{
		if (UUI_CharacterCreatePanel* UI_CharacterCreatePanel = GetParents<UUI_CharacterCreatePanel>())
		{
			// Create character
			if (!InState->IsCharacterExistInSlot(SlotPosition))
			{
				UE_LOG(MMOARPG, Display, TEXT("ClickedCharacter -> CreateCharacterFlow [Slot:%d]"), SlotPosition);

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
			// Enter the map with an existing character
			else if(UI_CharacterCreatePanel->GetHighlightButton() == this)
			{
				UE_LOG(MMOARPG, Display, TEXT("ClickedCharacter -> JumpDSServer [Slot:%d]"), SlotPosition);
				JumpDSServer();
			}
			// Set this button as highlighted
			else
			{
				int32 HighlightSlot = INDEX_NONE;
				if (UUI_CharacterButton* HighlightButton = UI_CharacterCreatePanel->GetHighlightButton())
				{
					HighlightSlot = HighlightButton->GetSlotPosition();
				}

				UE_LOG(MMOARPG, Display, TEXT("ClickedCharacter -> HighlightSelection [Slot:%d PrevHighlight:%d]"), SlotPosition, HighlightSlot);

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
	UE_LOG(MMOARPG, Display, TEXT("UUI_CharacterButton::JumpDSServer [Slot:%d]"), SlotPosition);

	if (UUI_CharacterCreatePanel* InUI_CharacterCreatePanel = GetParents<UUI_CharacterCreatePanel>())
	{
		InUI_CharacterCreatePanel->JumpDSServer(SlotPosition);
	}
	else
	{
		UE_LOG(MMOARPG, Error, TEXT("UUI_CharacterButton::JumpDSServer failed, CharacterCreatePanel is null [Slot:%d]"), SlotPosition);
	}
}
