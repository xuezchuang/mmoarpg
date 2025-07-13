// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_GameSettingsSystem.h"
#include "Components/CheckBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "GameSettings/UI_GameSetingsAudio.h"
#include "GameSettings/UI_GameSettingsGameSetting.h"
#include "GameSettings/UI_GameSettingsVideo.h"
#include "Core/SimpleGameUserSettings.h"
#include "SlateSound/SimpleSlateSound.h"

void UUI_GameSettingsSystem::NativeConstruct()
{
	Super::NativeConstruct();

	////ע�����ǵ����� ->UI����
	FSlateApplication& CurrentSlateApp = FSlateApplication::Get();
	CurrentSlateApp.InitializeSound(TSharedRef<FSimpleSlateSoundDevice>(new FSimpleSlateSoundDevice()));

	SaveButton->OnClicked.AddDynamic(this, &UUI_GameSettingsSystem::SaveSettings);
	RestoreDefaultButton->OnClicked.AddDynamic(this, &UUI_GameSettingsSystem::RestoreDefaultSettings);

	AudioSettingBox->OnCheckStateChanged.AddDynamic(this, &UUI_GameSettingsSystem::AudioSettingCheckBox);
	VideoSettingsBox->OnCheckStateChanged.AddDynamic(this, &UUI_GameSettingsSystem::VideoSettingsCheckBox);
	GameSettingsBox->OnCheckStateChanged.AddDynamic(this, &UUI_GameSettingsSystem::GameSettingsCheckBox);

	//��ȡ���ǵ���Ϸ����
	LoadSettings();
}

void UUI_GameSettingsSystem::AudioSettingCheckBox(bool ClickedWidget)
{
	SetSettingState(EGameSettingsType::AUDIO);
}

void UUI_GameSettingsSystem::VideoSettingsCheckBox(bool ClickedWidget)
{
	SetSettingState(EGameSettingsType::VIDEO);
}

void UUI_GameSettingsSystem::GameSettingsCheckBox(bool ClickedWidget)
{
	SetSettingState(EGameSettingsType::GAME);
}

void UUI_GameSettingsSystem::SaveSettings()
{
	GameSettingsVideo->SaveSettings();
	GameSetingsAudio->SaveSettings();
	GameSettingsGameSetting->SaveSettings();

	USimpleGameUserSettings::GetSimpleGameUserSettings()->ApplySettings(true);

	GameSettingsVideo->LoadSettings();
	GameSetingsAudio->LoadSettings();
	GameSettingsGameSetting->LoadSettings();
}

void UUI_GameSettingsSystem::LoadSettings()
{
	USimpleGameUserSettings::GetSimpleGameUserSettings()->LoadSettings(true);

	GameSettingsVideo->LoadSettings();
	GameSetingsAudio->LoadSettings();
	GameSettingsGameSetting->LoadSettings();
}

void UUI_GameSettingsSystem::RestoreDefaultSettings()
{
	USimpleGameUserSettings::GetSimpleGameUserSettings()->SetToDefaults();

	GameSettingsVideo->LoadSettings();
	GameSetingsAudio->LoadSettings();
	GameSettingsGameSetting->LoadSettings();
}

void UUI_GameSettingsSystem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUI_GameSettingsSystem::SetSettingState(EGameSettingsType Type)
{
	switch (Type)
	{
	case EGameSettingsType::VIDEO:
		AudioSettingBox->SetCheckedState(ECheckBoxState::Unchecked);
		GameSettingsBox->SetCheckedState(ECheckBoxState::Unchecked);
		VideoSettingsBox->SetCheckedState(ECheckBoxState::Checked);
		break;
	case EGameSettingsType::AUDIO:
		AudioSettingBox->SetCheckedState(ECheckBoxState::Checked);
		GameSettingsBox->SetCheckedState(ECheckBoxState::Unchecked);
		VideoSettingsBox->SetCheckedState(ECheckBoxState::Unchecked);
		break;
	case EGameSettingsType::GAME:
		AudioSettingBox->SetCheckedState(ECheckBoxState::Unchecked);
		GameSettingsBox->SetCheckedState(ECheckBoxState::Checked);
		VideoSettingsBox->SetCheckedState(ECheckBoxState::Unchecked);
		break;
	}

	SettingsListWitcher->SetActiveWidgetIndex((int32)Type);
}
