//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_ScreenShock.h"
#include "GameFramework/Character.h"
#include "Camera/CameraShakeBase.h"
#include "Object/Camera/ComboPlayerCameraManager.h"
#include "Object/Camera/ComboCameraShake.h"
#include "PerlinNoiseCameraShakePattern.h"

UAnimNotify_ScreenShock::UAnimNotify_ScreenShock(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,bCustomShockClass(false)
	,Scale(1.f)
	,PlaySpace(ECameraShakePlaySpace::CameraLocal)
	,UserPlaySpaceRot(FRotator::ZeroRotator)
{
	RootShakePattern = static_cast<UPerlinNoiseCameraShakePattern*>(ObjectInitializer.CreateDefaultSubobject(
		this,
		TEXT("RootShakePattern"),
		UPerlinNoiseCameraShakePattern::StaticClass(),
		nullptr,	// No class to create by default
		false,		// Not required
		false		// Not transient
	));
}

FString UAnimNotify_ScreenShock::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ScreenShock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld() && !InCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			if (APlayerController* InPlayerController = Cast<APlayerController>(InCharacter->GetController()))
			{
				if (InPlayerController->PlayerCameraManager)
				{
					if (!bCustomShockClass)
					{
						//保证里面有值
						if (!RootShakePattern)
						{
							return;
						}

						if (AComboPlayerCameraManager* InCameraManager = Cast<AComboPlayerCameraManager>(InPlayerController->PlayerCameraManager))
						{
							if (UCameraShakeBase* InShakeBase = InCameraManager->PlayCameraShake(
								UComboCameraShake::StaticClass(),
								FOnInitializeCameraShake::CreateUObject(this, &UAnimNotify_ScreenShock::InitializeCameraShake),
								Scale,
								PlaySpace,
								UserPlaySpaceRot))
							{

							}
						}
					}
					else
					{
						if (UCameraShakeBase* InShakeBase = InPlayerController->PlayerCameraManager->
							StartCameraShake(ShakeClass, Scale, PlaySpace, UserPlaySpaceRot))
						{

						}
					}
				}
			}
		}	
	}
}

void UAnimNotify_ScreenShock::InitializeCameraShake(UCameraShakeBase* InCameraShakeBase)
{
	if (InCameraShakeBase)
	{
		InCameraShakeBase->SetRootShakePattern(RootShakePattern);
	}
}