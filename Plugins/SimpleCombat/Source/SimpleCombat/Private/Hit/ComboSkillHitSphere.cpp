//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Hit/ComboSkillHitSphere.h"
#include "Components/SphereComponent.h"

AHitSphereCollision::AHitSphereCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HitDamage = CreateDefaultSubobject<USphereComponent>(TEXT("HitDamage"));
	HitDamage->SetupAttachment(RootComponent);
}

UPrimitiveComponent* AHitSphereCollision::GetHitDamage()
{
	return HitDamage;
}

// Called when the game starts or when spawned
void AHitSphereCollision::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHitSphereCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitSphereCollision::SetRadius(const float& InRadius)
{
	HitDamage->SetSphereRadius(InRadius);
}