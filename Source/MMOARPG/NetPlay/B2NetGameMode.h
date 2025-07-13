// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "SimpleNetChannelType.h"
#include "MMOARPGType.h"
#include "B2NetGameMode.generated.h"

class FSimpleChannel;
class ABladeIINetPlayer;

UCLASS()
class ABladeIINetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<ABladeIINetPlayer> OtherCharacterClass;


public:
	ABladeIINetGameMode();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void LoginCharacterUpdateKneadingRequest(int32 InUserID);

protected:
	virtual void PostLogin(APlayerController* NewPlayer);

private:
	void BindClientRcv();

	UFUNCTION()
	void LinkServerInfo(ESimpleNetErrorType InType, const FString& InMsg);

	void LinkServer();

	void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);
private:
	FDelegateHandle RecvDelegate;

	TMap<uint32, ABladeIINetPlayer*>	MapOtherCharacter;
};



