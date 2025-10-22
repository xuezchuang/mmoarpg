// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGMonster.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MMOARPGCharacter.h"
#include "MMOARPGEnemyController.h"
#include "Components/WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "EnemyInfoWidget.h"
#include "MMOARPGGameMode.h"

#define LOCTEXT_NAMESPACE "EnemyNameSpace"

void AMMOARPGMonster::UpdateHealthBar()
{
	//EnemyInfoWidget->HealthBar->SetPercent(CurrentHealth / TotalHealth);
	//if (bSelected)
	//{
	//	SelectingCharacter->MainUserWidget->EnemyHpBar->SetPercent(CurrentHealth / TotalHealth);
	//	SelectingCharacter->MainUserWidget->EnemyHpText->SetText(FText::Format(LOCTEXT("EnemyNameSpace", "{0}/{1}"), FText::AsNumber(FMath::RoundHalfToZero(CurrentHealth)), FText::AsNumber(TotalHealth)));
	//}
}

AMMOARPGMonster::AMMOARPGMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AMMOARPGEnemyController::StaticClass();
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (AIPerceptionComp)
	{
		SightConfig->SightRadius = 800;
		SightConfig->LoseSightRadius = 2000;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	}

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());

	AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMMOARPGMonster::OnSightPerceptionUpdate);
	EnemyWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyWidgetComp"));

	static ConstructorHelpers::FClassFinder<UEnemyInfoWidget> EIW(TEXT("WidgetBlueprint'/Game/UI/Game/WBP_EnemyInfo.WBP_EnemyInfo_C'"));
	if (EIW.Succeeded())
	{
		EnemyWidgetComp->SetWidgetClass(EIW.Class);
	}
	EnemyWidgetComp->SetDrawSize(FVector2D(200.0f, 60.0f));
	EnemyWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyWidgetComp->SetTwoSided(true);
	EnemyWidgetComp->SetVisibility(false);

	EnemyWidgetComp->SetupAttachment(RootComponent);
	//EnemyWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	EnemyWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	ShowUICollision = CreateDefaultSubobject<USphereComponent>(TEXT("ShowUICollison"));
	ShowUICollision->SetupAttachment(RootComponent);
	ShowUICollision->OnComponentBeginOverlap.AddDynamic(this, &AMMOARPGMonster::OnBeginOverlap);
	ShowUICollision->OnComponentEndOverlap.AddDynamic(this, &AMMOARPGMonster::OnEndOverlap);
	HitArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("HitArrow"));
	HitArrow->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMMOARPGMonster::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		Destroy();
		return;
	}

	// 获取当前 GameMode（只在服务器有效）
	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (!GameMode || GameMode->IsA(AMMOARPGGameMode::StaticClass()))
	{
		// 如果不是 MMOARPGGameMode，就销毁自己
		Destroy();
		return;
	}

	EnemyInfoWidget = Cast<UEnemyInfoWidget>(EnemyWidgetComp->GetUserWidgetObject());
	MyController = Cast<AMMOARPGEnemyController>(GetController());
	if (MyController)
		MyController->Patrol();
	StartLocation = GetActorLocation();
	EnemyWidgetComp->SetVisibility(bInShowRange);
	InitWidgetText();
	CurrentHealth = TotalHealth;
	UpdateHealthBar();
}

void AMMOARPGMonster::OnSightPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	if (Info.bAggressive && !MyController->bWasAggroed)
	{
		for (auto Actor : UpdatedActors)
		{
			if (Cast<AMMOARPGCharacterBase>(Actor))
			{
				UE_LOG(LogTemp, Warning, TEXT("OnSightPerception"));
				MyController->OnAggroedPulled(Cast<AMMOARPGCharacterBase>(Actor));
			}
		}
	}
}

void AMMOARPGMonster::OnNotifyHit()
{
	AttackRay();
}

void AMMOARPGMonster::AttackRay()
{
	int Range = 250;
	StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetCapsuleComponent()->GetForwardVector() * Range;

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel2, QueryParams))
	{
		AMMOARPGCharacter* Character = Cast<AMMOARPGCharacter>(HitResult.GetActor());
		if (Character)
		{
			//IDamageableInterface* DamageableInterface = Cast<IDamageableInterface>(Character);
			//if (DamageableInterface)
			//{
			//	DamageableInterface->OnReceiveDamage(BaseDamage, DamageType, Element, CritChance, this, nullptr);
			//}
		}
	}
}

void AMMOARPGMonster::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Info.bDead)
	{
		//if (Cast<ARBaseCharacter>(OtherActor) && !EnemyWidgetComp->IsVisible())
		//{
		//	bInShowRange = true;
		//	EnemyWidgetComp->SetVisibility(true);
		//}
	}
}

void AMMOARPGMonster::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Info.bDead)
	{
	//	bInShowRange = false;
	//	if (Cast<ARBaseCharacter>(OtherActor) && EnemyWidgetComp->IsVisible())
	//	{
	//		EnemyWidgetComp->SetVisibility(false);
	//	}
	}
}

void AMMOARPGMonster::InitWidgetText()
{
	EnemyInfoWidget->LevelName->SetText(FText::Format(LOCTEXT("EnemyNameSpace", "[Lv{0}.{1}]"), FText::AsNumber(Info.Level), Info.Name));
	FLinearColor CurrentColor;
	if (Info.bAggressive)
	{
		CurrentColor = FLinearColor::Red;
	}
	else
	{
		CurrentColor = FLinearColor::White;
	}
	EnemyInfoWidget->SetColorAndOpacity(CurrentColor);
}

#undef LOCTEXT_NAMESPACE