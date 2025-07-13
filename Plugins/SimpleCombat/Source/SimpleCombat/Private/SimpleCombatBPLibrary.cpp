//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "SimpleCombatBPLibrary.h"
#include "SimpleCombat.h"
#include "Manage/ComboCountManage.h"
#include "Actor/ResidualShadowActor.h"

USimpleCombatBPLibrary::USimpleCombatBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USimpleCombatBPLibrary::ComboPlay(UObject* WorldContextObject, TSubclassOf<UUI_ComboCount> InClass)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FComboCountManage::Get()->Play(World, InClass);
	}
}

void USimpleCombatBPLibrary::ComboTextDestroy()
{
	FComboCountManage::Destroy();
}

AResidualShadowActor* USimpleCombatBPLibrary::SpawnResidualShadow(
	UObject* WorldContextObject,
	TSubclassOf<AResidualShadowActor>ResidualShadowActorClass,
	USkeletalMeshComponent* InComponentToCopy,
	float InZOffset,
	const FVector& Location, const FRotator& InRot, float InLiftTime)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector NewLocation = FVector(Location.X, Location.Y, Location.Z + InZOffset);
		FRotator NewRot = FRotator(InRot.Pitch, InRot.Yaw - 90.f, InRot.Roll);

		if (AResidualShadowActor* InRS = World->SpawnActor<AResidualShadowActor>(
			ResidualShadowActorClass,NewLocation,NewRot, SpawnParameters))
		{
			InRS->SetLifeSpan(InLiftTime);
			InRS->UpdateResidualShadow(InComponentToCopy);

			return InRS;
		}
	}

	return NULL;
}