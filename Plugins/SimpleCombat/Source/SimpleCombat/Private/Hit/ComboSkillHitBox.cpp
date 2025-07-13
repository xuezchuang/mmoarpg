//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Hit/ComboSkillHitBox.h"
#include "Components/BoxComponent.h"

AHitBoxCollision::AHitBoxCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HitDamage = CreateDefaultSubobject<UBoxComponent>(TEXT("HitDamage"));
	HitDamage->SetupAttachment(RootComponent);
}

UPrimitiveComponent* AHitBoxCollision::GetHitDamage()
{
	return HitDamage;
}

// Called when the game starts or when spawned
void AHitBoxCollision::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHitBoxCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitBoxCollision::SetBoxExtent(const FVector& InNewBoxExtent)
{
	HitDamage->SetBoxExtent(InNewBoxExtent);
}