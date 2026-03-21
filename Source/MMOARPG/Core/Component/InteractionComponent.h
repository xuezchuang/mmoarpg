// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../MMOARPGGameType.h"
#include "../../Data/FItemData.h"
//#include "Engine/Texture2D.h"
#include "InteractionComponent.generated.h"

class AMMOARPGCharacterBase;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UCameraComponent;
class UUI_Vendor;
class AInventoryGameState;
class UWidgetComponent;
class UUI_Interaction;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class MMOARPG_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, Category = "Interaction|UI")
	//TSubclassOf<UUI_Interaction> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "BP_Setting")
	TMap<E_InTeractableType, TSubclassOf<UUserWidget>> InteractionUIMap;

public:
	UPROPERTY(EditInstanceOnly, Category = BP_Setting)
	E_DataTableType DataTableType;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = BP_Setting)
	E_InTeractableType InteractionType;

	virtual void InitializeComponent() override;

	void StartHold();
	void StopHold();
	void Interaction();
public:
	// Sets default values for this component's properties
	UInteractionComponent();

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void Print(float InTime,const FString &InString);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Initialize(UWidget* InInteractionWidget);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowTitle(bool Visibility);


	//void ResetFly();

	//UFUNCTION(BlueprintCallable, Category = BP_Setting)
	//FSlateColor GetRarityColor(E_ItemRarity rarity);

	//UFUNCTION(BlueprintCallable, Category = BP_Setting)
	//UTexture2D* GetStatTexture(E_StatCategory stat);

	//UFUNCTION()
	//void Landed(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// 
	//UFUNCTION(BlueprintCallable, Category = BFL_Inventory, meta = (WorldContext = "WorldContextObject"))
	//static UInteractionComponent* GetInventoryComponent(const UObject* WorldContextObject);
protected:
	//UPROPERTY()
	//TWeakObjectPtr<AMMOARPGCharacterBase> MMOARPGCharacterBase;
	
	// 按住需要的时间（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float HoldTime = 1.0f;
private:
	//AInventoryGameState* m_InventoryGameState = NULL;
	UPROPERTY()
	UUI_Interaction* InteractionWidget;

	float HoldProgress = 0.f;

	// 是否正在按住
	bool bIsHolding = false;

	// 当前按下的按键名（可用于多键）
	FKey HoldingKey;


};
