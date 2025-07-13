//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResidualShadowActor.generated.h"

class UPoseableMeshComponent;
class USkeletalMeshComponent;
UCLASS()
class SIMPLECOMBAT_API AResidualShadowActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResidualShadow", meta = (AllowPrivateAccess = "true"))
	UPoseableMeshComponent* ResidualShadowComponent;
public:	
	// Sets default values for this actor's properties
	AResidualShadowActor();

	void UpdateResidualShadow(USkeletalMeshComponent* InComponentToCopy);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
