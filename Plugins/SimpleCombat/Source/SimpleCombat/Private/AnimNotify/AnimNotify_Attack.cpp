//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_Attack.h"
#include "Hit/ComboSkillHitBox.h"
#include "Hit/ComboSkillHitCapsule.h"
#include "Hit/ComboSkillHitCustom.h"
#include "Hit/ComboSkillHitSphere.h"
#include "GameFramework/Character.h"

UAnimNotify_Attack::UAnimNotify_Attack()
	:Super()
{
	HitObjectClass = AHitBoxCollision::StaticClass();
	bEnableClientPrediction = false;
	bHitBox = true;
	bHitCapsule = false;
	bHitCustom = false;
	bHitSphere = false;
	bEnableBuffTag = false;

	BoxExtent = FVector(32.f);
	SphereRadius = 32.f;
	CapsuleHalfHeight = 44.f;
	CapsuleRadius = 22.f;
	LiftTime = 4.f;
	InSocketName = TEXT("OpenFire");

//	Direction = FVector::ZeroVector;
}
#if WITH_EDITORONLY_DATA
FLinearColor UAnimNotify_Attack::GetEditorColor()
{
	return FLinearColor::Red;
}
#endif

#if WITH_EDITOR
void UAnimNotify_Attack::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("HitObjectClass"))
	{
		if (HitObjectClass.GetDefaultObject()->IsA(AHitBoxCollision::StaticClass()))
		{
			bHitBox = true;
			bHitCapsule = false;
			bHitCustom = false;
			bHitSphere = false;
		}
		else if (HitObjectClass.GetDefaultObject()->IsA(AHitCapsuleCollision::StaticClass()))
		{
			bHitCapsule = true;
			bHitBox = false;
			bHitCustom = false;
			bHitSphere = false;
		}
		else if (HitObjectClass.GetDefaultObject()->IsA(AHitCustiomCollision::StaticClass()))
		{
			bHitCustom = true;
			bHitBox = false;
			bHitCapsule = false;
			bHitSphere = false;
		}
		else if (HitObjectClass.GetDefaultObject()->IsA(AHitSphereCollision::StaticClass()))
		{
			bHitSphere = true;
			bHitBox = false;
			bHitCapsule = false;
			bHitCustom = false;
		}
	}
}
#endif

FString UAnimNotify_Attack::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	//AAnimationEditorPreviewActor
	if (AActor* InSimpleCombatCharacter = Cast<AActor>(MeshComp->GetOuter()))
	{
		FVector ComponentLocation = MeshComp->GetSocketLocation(InSocketName);
		FRotator ComponentRotation = MeshComp->GetSocketRotation(InSocketName);

		if (InSimpleCombatCharacter->GetWorld())
		{
			if (bEnableClientPrediction || InSimpleCombatCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
			{
				FTransform Transform((ComponentRotation + RotationOffset).Quaternion(), ComponentLocation);
				
				if (AHitCollision* HitCollision = InSimpleCombatCharacter->GetWorld()->SpawnActorDeferred<AHitCollision>(
					HitObjectClass, 
					Transform,
					NULL,
					Cast<APawn>(InSimpleCombatCharacter),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
				{
					HitCollision->PreInitCollision(InSimpleCombatCharacter);

					//注册
					HitCollision->SetHitID(HitID);
					if (bEnableBuffTag)
					{
						HitCollision->SetBuffs(BuffTags);
					}
					else
					{
						HitCollision->SetBuffs(Buffs);
					}

					//HitCollision->Collision(true);

					//碰撞方面
					if (HitCollision->GetHitDamage())
					{
						FVector RelativeLocation = HitCollision->GetHitDamage()->GetRelativeLocation();

						HitCollision->SetLifeSpan(LiftTime);
						HitCollision->SetHitDamageRelativePosition(RelativeLocation + RelativeOffsetLocation);

						if (AHitBoxCollision* InBox = Cast<AHitBoxCollision>(HitCollision))
						{
							InBox->SetBoxExtent(BoxExtent);
						}
						else if (AHitCapsuleCollision* InCapsule = Cast<AHitCapsuleCollision>(HitCollision))
						{
							InCapsule->SetCapsuleHalfHeight(CapsuleHalfHeight);
							InCapsule->SetCapsuleRadius(CapsuleRadius);
						}
						else if (AHitSphereCollision* InSphere = Cast<AHitSphereCollision>(HitCollision))
						{
							InSphere->SetRadius(SphereRadius);
						}
						else if (AHitCustiomCollision* InCustiom = Cast<AHitCustiomCollision>(HitCollision))
						{

						}
					}

					//结束延迟生成
					HitCollision->FinishSpawning(Transform);
				//HitCollision->SetActorRotation(ComponentRotation + RotationOffset);
					//必须保证对象生成后再附加，否则附加会失败
					if (bBind)
					{
						HitCollision->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
					}
				}
			}
		}
	}
}