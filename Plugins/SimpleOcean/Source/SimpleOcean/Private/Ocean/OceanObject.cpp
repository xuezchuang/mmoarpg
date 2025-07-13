//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Ocean/OceanObject.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/SimpleOceanInterface.h"

AOceanObject::AOceanObject()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshStaticClass(TEXT("/Engine/BasicShapes/Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> InOceanMatClass(TEXT("/SimpleOcean/OceanAsset/InOceanMat_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OutOceanMatClass(TEXT("/SimpleOcean/OceanAsset/OutOceanMat_Inst"));

	BoxCollisionBoundComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionBound"));
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	WaterPlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterPlaneMesh"));
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneRootComponent;
	
	FVector VectorSize = FVector(50.f, 50.f, 164.f);

	WaterPlaneMeshComponent->SetRelativeScale3D(VectorSize);
	WaterPlaneMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WaterPlaneMeshComponent->SetupAttachment(RootComponent);
	BoxCollisionBoundComponent->SetupAttachment(RootComponent);
	PostProcessComponent->SetupAttachment(BoxCollisionBoundComponent);

	WaterPlaneMeshComponent->SetStaticMesh(PlaneMeshStaticClass.Object);
	WaterPlaneMeshComponent->SetMaterial(0,InOceanMatClass.Object);

	PostProcessComponent->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, OutOceanMatClass.Object));

	BoxCollisionBoundComponent->SetRelativeLocation(FVector(0.f, 0.f, -(VectorSize.Z * VectorSize.Z)));
	BoxCollisionBoundComponent->SetRelativeScale3D(VectorSize);
	BoxCollisionBoundComponent->SetBoxExtent(VectorSize);

	PostProcessComponent->BlendRadius = 10.f;
	PostProcessComponent->bUnbound = false;
}

void AOceanObject::BeginPlay()
{
	Super::BeginPlay();

	if (BoxCollisionBoundComponent)
	{
		BoxCollisionBoundComponent->OnComponentBeginOverlap.AddDynamic(this,&AOceanObject::BeginOverlapWater);
		BoxCollisionBoundComponent->OnComponentEndOverlap.AddDynamic(this, &AOceanObject::EndOverlapWater);
	}
}

void AOceanObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOceanObject::BeginOverlapWater(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ISimpleOceanInterface*InCharacter = Cast<ISimpleOceanInterface>(OtherActor))
	{
		InCharacter->OceanObjectEnter(this);
	}
}

void AOceanObject::EndOverlapWater(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ISimpleOceanInterface* InCharacter = Cast<ISimpleOceanInterface>(OtherActor))
	{
		InCharacter->OceanObjectLeave(this);
	}
}