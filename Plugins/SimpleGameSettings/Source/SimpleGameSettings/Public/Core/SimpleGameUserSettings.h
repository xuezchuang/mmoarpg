// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SimpleGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SIMPLEGAMESETTINGS_API USimpleGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

	//��������
	UPROPERTY(config)
	float BackgroundSound;

	//��Ϸ����
	UPROPERTY(config)
	float GameSound;

	//��Ϸ����
	UPROPERTY(config)
	float UISound;

	//�Ƿ�֧����������
	UPROPERTY(config)
	bool bSupportPhysX;
public:
	USimpleGameUserSettings();

	virtual void ApplySettings(bool bCheckForCommandLineOverrides)override;
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void SaveSettings() override;
	virtual void SetToDefaults()override;
	void SetCurrentLanguage(const FString &NewLanguage);

	void SeveTDIni();
	bool LoadTDIni();

	static USimpleGameUserSettings* GetSimpleGameUserSettings();

	FString GetCurrentLanguageType();
	class UWorld* GetWorld() const;

	void SetBackgroundSound(float VolumeMultiplier);

	void SetGameSound(float VolumeMultiplier);

	void SetUISound(float VolumeMultiplier);

	float GetBackgroundSound() const;

	float GetGameSound() const;

	float GetUISound() const;

	bool GetAllAmbientSound(TArray<class AAmbientSound*> &Sounds);

	bool SpawnSoundAtLocation(USoundBase* CharacterSound,const FVector &Location);

protected:
	bool GetAllSoundClass(TArray<class USoundClass*> &Sounds);
};
