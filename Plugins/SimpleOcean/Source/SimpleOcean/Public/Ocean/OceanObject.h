//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "OceanObject.generated.h"

class UBoxComponent;
class UPostProcessComponent;
class UStaticMeshComponent;
class USceneComponent;
/**
 *
 */
UCLASS()
class SIMPLEOCEAN_API AOceanObject : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(Category = MMOARPGCharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollisionBoundComponent;

	UPROPERTY(Category = MMOARPGCharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponent;

	UPROPERTY(Category = MMOARPGCharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WaterPlaneMeshComponent;

	UPROPERTY(Category = MMOARPGCharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRootComponent;
public:
	AOceanObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void BeginOverlapWater(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlapWater(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
