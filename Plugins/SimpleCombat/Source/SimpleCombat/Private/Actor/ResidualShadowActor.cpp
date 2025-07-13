//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Actor/ResidualShadowActor.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
AResidualShadowActor::AResidualShadowActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ResidualShadowComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("ResidualShadowComponent"));
	RootComponent = ResidualShadowComponent;

}

// Called when the game starts or when spawned
void AResidualShadowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResidualShadowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AResidualShadowActor::UpdateResidualShadow(USkeletalMeshComponent* InComponentToCopy)
{
	ResidualShadowComponent->CopyPoseFromSkeletalComponent(InComponentToCopy);
}