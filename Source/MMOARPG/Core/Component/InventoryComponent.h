// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../MMOARPGGameType.h"
#include "../../Data/FItemData.h"
//#include "Engine/Texture2D.h"
#include "InventoryComponent.generated.h"

class AMMOARPGCharacterBase;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UCameraComponent;
class UUI_Vendor;
class AInventoryGameState;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class MMOARPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = BP_Setting)
	TSubclassOf<UUI_Vendor> Vendor_BPClass;

	UPROPERTY(EditDefaultsOnly, Category = BP_Setting)
	TMap<E_StatCategory,UTexture2D*> mapStatTexture;

public:

	UPROPERTY(BlueprintReadOnly, Category = BP_Setting)
	UUI_Vendor* m_UIShop;

	UPROPERTY(BlueprintReadWrite, Category = BP_Setting)
	bool m_CanShop = false;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	//bool m_CanShop2 = false;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	//EDodgeFly DodgeFly;


	virtual void InitializeComponent() override;

	//FResetBool bFastFly;
	//FResetBool bLand;
public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = BP_Setting)
	void ShowVendor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void Print(float InTime,const FString &InString);

	void Reset();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetFly();

	UFUNCTION(BlueprintCallable, Category = BP_Setting)
	FSlateColor GetRarityColor(E_ItemRarity rarity);

	UFUNCTION(BlueprintCallable, Category = BP_Setting)
	UTexture2D* GetStatTexture(E_StatCategory stat);

	//UFUNCTION()
	//void Landed(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// 
	UFUNCTION(BlueprintPure, Category = BFL_Inventory, meta = (WorldContext = "WorldContextObject"))
	static UInventoryComponent* GetInventoryComponent(const UObject* WorldContextObject);
protected:
	UPROPERTY()
	TWeakObjectPtr<AMMOARPGCharacterBase> MMOARPGCharacterBase;

	//UPROPERTY()
	//TWeakObjectPtr<UCapsuleComponent> CapsuleComponent;

	//UPROPERTY()
	//TWeakObjectPtr<UCameraComponent> CameraComponent;

	//UPROPERTY()
	//TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	//UPROPERTY()
	//FRotator LastRotator;
private:
	AInventoryGameState* m_InventoryGameState = NULL;
};
