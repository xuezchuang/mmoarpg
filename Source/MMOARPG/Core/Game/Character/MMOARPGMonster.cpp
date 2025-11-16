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
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "EnemyInfoWidget.h"
#include "NetPlay/B2NetGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ProgressBar.h"
#include "Component/SelectableComponent.h"

#define LOCTEXT_NAMESPACE "EnemyNameSpace"

void AMMOARPGMonster::UpdateHealthBar()
{
	if (EnemyInfoWidget)
	{
		EnemyInfoWidget->HealthBar->SetPercent(Info.CurrentHealth / Info.TotalHealth);
		//if (bSelected)
		{
			//SelectingCharacter->MainUserWidget->EnemyHpBar->SetPercent(CurrentHealth / TotalHealth);
			//SelectingCharacter->MainUserWidget->EnemyHpText->SetText(FText::Format(LOCTEXT("EnemyNameSpace", "{0}/{1}"), FText::AsNumber(FMath::RoundHalfToZero(CurrentHealth)), FText::AsNumber(TotalHealth)));
		}
	}
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

	EnemyWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyWidgetComp"));
	EnemyWidgetComp->SetupAttachment(RootComponent);
	EnemyWidgetComp->SetWidgetSpace(EWidgetSpace::World);     // 血条一般用 World 空间
	EnemyWidgetComp->SetDrawAtDesiredSize(true);              // 让 UMG 自己决定大小
	EnemyWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyWidgetComp->SetTwoSided(true);
	EnemyWidgetComp->SetVisibility(false);
	EnemyWidgetComp->SetDrawSize(FVector2D(200.0f, 60.0f));

	//EnemyWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyWidgetComp"));

	//static ConstructorHelpers::FClassFinder<UEnemyInfoWidget> EIW(TEXT("WidgetBlueprint'/Game/UI/Game/WBP_EnemyInfo.WBP_EnemyInfo_C'"));
	//if (EIW.Succeeded())
	//{
	//	EnemyWidgetComp->SetWidgetClass(EIW.Class);
	//}
	//EnemyWidgetComp->SetDrawSize(FVector2D(200.0f, 60.0f));
	//EnemyWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//EnemyWidgetComp->SetTwoSided(true);
	//EnemyWidgetComp->SetVisibility(false);

	//EnemyWidgetComp->SetupAttachment(RootComponent);
	//EnemyWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	//EnemyWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	ShowUICollision = CreateDefaultSubobject<USphereComponent>(TEXT("ShowUICollison"));
	ShowUICollision->SetupAttachment(RootComponent);
	ShowUICollision->OnComponentBeginOverlap.AddDynamic(this, &AMMOARPGMonster::OnBeginOverlap);
	ShowUICollision->OnComponentEndOverlap.AddDynamic(this, &AMMOARPGMonster::OnEndOverlap);
	HitArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("HitArrow"));
	HitArrow->SetupAttachment(RootComponent);

	SelectableComp = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComp"));
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

	if (EnemyWidgetClass)
	{
		EnemyWidgetComp->SetWidgetClass(EnemyWidgetClass);
	}

	// 获取当前 GameMode（只在服务器有效）
	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (!GameMode)
	{
		// 如果不是 MMOARPGGameMode，就销毁自己
		Destroy();
		return;
	}
	if (!GameMode->IsA(ABladeIINetGameMode::StaticClass()))
	{
		AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMMOARPGMonster::OnSightPerceptionUpdate);
	}

	if (SelectableComp)
	{
		SelectableComp->OnSelectedEvent.AddDynamic(this, &AMMOARPGMonster::HandleSelected);
		SelectableComp->OnSelectionEndEvent.AddDynamic(this, &AMMOARPGMonster::HandleSelectionEnd);
	}

	EnemyInfoWidget = Cast<UEnemyInfoWidget>(EnemyWidgetComp->GetUserWidgetObject());
	MyController = Cast<AMMOARPGEnemyController>(GetController());
	if (MyController)
		MyController->Patrol();
	StartLocation = GetActorLocation();
	AdjustZToGround(StartLocation);
	SetActorLocation(StartLocation, true);

	EnemyWidgetComp->SetVisibility(bInShowRange);
	if (EnemyInfoWidget)
	{
		InitWidgetText();
	}
	CurrentHealth = Info.TotalHealth;
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

void AMMOARPGMonster::ApplyNetAnimParams(float InSpeed, const FVector& InVelDirWS, ENetMonsterAction InAction, float DeltaSeconds)
{
    // 速度平滑（避免抖动）
    const float TargetSpeed = FMath::Max(0.f, InSpeed);
    VisualSpeed = FMath::FInterpTo(VisualSpeed, TargetSpeed, DeltaSeconds, 8.f);

    // 方向：把世界速度方向投影到水平面，换算成相对朝向角（供 2D BlendSpace 使用）
    FVector FlatDir = InVelDirWS; FlatDir.Z = 0.f;
    const float DirDeg = FlatDir.IsNearlyZero()
        ? 0.f
        : UKismetMathLibrary::NormalizedDeltaRotator(FlatDir.Rotation(), GetActorRotation()).Yaw;
    VisualDirection = FMath::FInterpTo(VisualDirection, DirDeg, DeltaSeconds, 8.f);

    VisualAction = InAction;
    bVisualMoving = (VisualSpeed > 3.f); // 视作在动的阈值，按手感调
}

void AMMOARPGMonster::HandleSelected()
{

}

void AMMOARPGMonster::HandleSelectionEnd()
{

}

void AMMOARPGMonster::AdjustZToGround(FVector& Pos)
{
	const FVector Start = Pos + FVector(0, 0, 2000.f);
	const FVector End = Pos + FVector(0, 0, -2000.f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AdjustZToGround), false, this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		// 2) 计算“理想的胶囊中心Z = 地面高度 + 胶囊半高”
		float HalfHeight = 88.f; // 备用默认
		if (const UCapsuleComponent* Cap = GetCapsuleComponent())
		{
			HalfHeight = Cap->GetScaledCapsuleHalfHeight();    // 注意用 *Scaled*，考虑缩放和蹲伏
		}

		// 给一点余量，避免浮点误差导致轻微穿插
		const float GroundSnapEpsilon = 0.5f;

		Pos.Z = Hit.ImpactPoint.Z + HalfHeight + GroundSnapEpsilon;

#if WITH_EDITOR
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Green, false, 1.0f, 0, 1.0f);
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, 1.0f);
#endif
	}
	// else：没打到地面就不改Z（可选：保底用当前Z或场景最低限）
}


#undef LOCTEXT_NAMESPACE