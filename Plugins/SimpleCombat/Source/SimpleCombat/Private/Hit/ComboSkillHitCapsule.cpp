//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Hit/ComboSkillHitCapsule.h"
#include "Components/CapsuleComponent.h"

AHitCapsuleCollision::AHitCapsuleCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HitDamage = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitDamage"));
	HitDamage->SetupAttachment(RootComponent);
}

UPrimitiveComponent* AHitCapsuleCollision::GetHitDamage()
{
	return HitDamage;
}

void AHitCapsuleCollision::SetCapsuleHalfHeight(float InNewCapsuleHalfHeight)
{
	HitDamage->SetCapsuleHalfHeight(InNewCapsuleHalfHeight);
}

void AHitCapsuleCollision::SetCapsuleRadius(float InNewCapsuleRadius)
{
	HitDamage->SetCapsuleRadius(InNewCapsuleRadius);
}
// Called when the game starts or when spawned
void AHitCapsuleCollision::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHitCapsuleCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}