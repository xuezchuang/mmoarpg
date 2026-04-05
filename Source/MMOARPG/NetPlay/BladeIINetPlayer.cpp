// Copyright Epic Games, Inc. All Rights Reserved.

#include "BladeIINetPlayer.h"
#include "ThreadManage.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Core/Component/FlyComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MMOARPGMacroType.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPG.h"
#include "AIModule/Classes/AIController.h"
#include "NavigationSystem/Public/NavigationSystem.h"
//////////////////////////////////////////////////////////////////////////
// ABladeIINetPlayer

ABladeIINetPlayer::ABladeIINetPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABladeIINetPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ABladeIINetPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Remote player EndPlay [uid:%u actor:%s reason:%d]"),
		RemotePlayerId, *GetNameSafe(this), static_cast<int32>(EndPlayReason));
	Super::EndPlay(EndPlayReason);
}

void ABladeIINetPlayer::SubscribeEvents()
{
	//if(UMMOARPGGameInstance* InGameInstance = GetGameInstance<UMMOARPGGameInstance>())
	//{
	//	if(InGameInstance->GetClient() && InGameInstance->GetClient()->GetController() && InGameInstance->GetClient()->GetChannel() &&
	//	   InGameInstance->GetClient()->GetChannel()->GetConnetion() && InGameInstance->GetClient()->GetChannel()->GetConnetion()->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
	//	{
	//		LinkInit();
	//		RecvDelegate = InGameInstance->GetClient()->GetController()->RecvDelegate.AddLambda(
	//			[&](uint32 ProtocolNumber, FSimpleChannel* Channel)
	//			{
	//				this->RecvProtocol(ProtocolNumber, Channel);
	//			});
	//	}
	//	else
	//	{
	//		GThread::Get()->GetCoroutines().BindLambda(0.5f, [&]()
	//												   {
	//													   BindClientRcv();
	//												   });
	//	}
	//}
	//else
	//{
	//	GThread::Get()->GetCoroutines().BindLambda(0.5f, [&]()
	//											   {
	//												   BindClientRcv();
	//											   });
	//}
}

void ABladeIINetPlayer::UnsubscribeEvents()
{

}

void ABladeIINetPlayer::UpdateMoveData(const S_MOVE_ROLE* rMove)
{
	m_TargetPos.X = rMove->targetpos.x;
	m_TargetPos.Y = rMove->targetpos.y;
	m_TargetPos.Z = rMove->targetpos.z;
	m_TargetSpeedTemp = static_cast<float>(rMove->speed) / 100.f;
	m_TargetFace = static_cast<float>(rMove->face) / 100.f;

	m_CurFace = GetActorRotation().Yaw;
	m_CurPos = GetActorLocation();
	//AAIController* AIController = Cast<AAIController>(GetController());
	//UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	//FNavLocation NavLocation;
	//if(NavSystem && NavSystem->ProjectPointToNavigation(m_TargetPos, NavLocation))
	//{
	//	if(AIController)
	//	{
	//		EPathFollowingRequestResult::Type rlt = AIController->MoveToLocation(m_TargetPos);
	//		GEngine->AddOnScreenDebugMessage(1111, 20.0f, FColor::Yellow, FString::Printf(TEXT("Curpos[%f-%f-%f],TarPos[%f-%f-%f],rlt[%d]"),
	//			m_CurPos.X, m_CurPos.Y, m_CurPos.Z, m_TargetPos.X, m_TargetPos.Y, m_TargetPos.Z, rlt));
	//	}
	//}
	//else
	//{
	//	UE_LOG(MMOARPG, Warning, TEXT("Target location is not on NavMesh.TarPos[%f-%f-%f]"), m_TargetPos.X, m_TargetPos.Y, m_TargetPos.Z);
	//}
	if(m_TargetSpeedTemp > 0.0)
	{
		m_CurSpeed = m_TargetSpeedTemp;
	}
	//UE_LOG(MMOARPG, Display, TEXT("pos[%f,%f,%f] targetpos[%f,%f,%f]"), m_CurPos.X, m_CurPos.Y, m_CurPos.Z, m_TargetPos.X, m_TargetPos.Y, m_TargetPos.Z);
}

void ABladeIINetPlayer::UpdateBaseData(const S_ROLE_O_BASE* RoleBase)
{
	m_TargetPos.X = RoleBase->pos.x;
	m_TargetPos.Y = RoleBase->pos.y;
	m_TargetPos.Z = RoleBase->pos.z;
	m_TargetFace = static_cast<float>(RoleBase->face) / 100.f;
	m_CurFace = m_TargetFace;
	m_TargetSpeedTemp = 0.0f;
	m_CurSpeed = 0.0f;
	SetActorLocation(m_TargetPos);
	SetActorRotation(FRotator(0.f, m_TargetFace, 0.f));

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->Velocity = FVector::ZeroVector;
		MovementComponent->SetMovementMode(MOVE_Walking);
	}
}

void ABladeIINetPlayer::SetRemotePlayerId(uint32 InRemotePlayerId)
{
	RemotePlayerId = InRemotePlayerId;
}

uint32 ABladeIINetPlayer::GetRemotePlayerId() const
{
	return RemotePlayerId;
}

void ABladeIINetPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_TargetPos.Equals(GetActorLocation(), 2.f) || m_TargetSpeedTemp > 0.f)
	{
		UpDateMove(DeltaTime);
	}
}

void ABladeIINetPlayer::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	
}

void ABladeIINetPlayer::UpDateMove(float DeltaTime)
{
	m_CurFace = GetActorRotation().Yaw;
	float RealYaw = FMath::Lerp(m_CurFace, m_TargetFace, FMath::Clamp(DeltaTime * 16.f, 0.f, 1.f));
	SetActorRotation(FRotator(0, RealYaw, 0));
	//
	m_CurPos = GetActorLocation();
	FVector NewLocation = m_CurPos;
	FVector dir = (m_TargetPos - m_CurPos).GetSafeNormal(0.0001);
	float distance = FVector::Distance(m_CurPos, m_TargetPos);
	if(distance > 10)
	{
		m_TargetIndex = 0;
		float CurSpeed = 0;
		if(distance > 250.0)
		{
			CurSpeed = 900.0;
		}
		else
		{
			CurSpeed = m_CurSpeed + DeltaTime / 1.8;
		}
		NewLocation = m_CurPos + CurSpeed * DeltaTime * dir;
		//UE_LOG(MMOARPG, Display, TEXT("curpos[%f,%f,%f]"), Location.X, Location.Y, Location.Z);
		SetActorLocation(NewLocation);
		SyncRemoteMovementState(m_CurPos, NewLocation, DeltaTime);
		return;
	}

	NewLocation = m_TargetPos;
	SetActorLocation(NewLocation);
	SyncRemoteMovementState(m_CurPos, NewLocation, DeltaTime);
	if(m_TargetSpeedTemp == 0.0)
	{
		m_TargetIndex++;
		if(m_TargetIndex == 1)
		{
			m_CurSpeed = 405.0;
		}
		else if(m_TargetIndex == 2)
		{
			m_CurSpeed = 263.0;
		}
		else if(m_TargetIndex == 3)
		{
			m_CurSpeed = 159.0;
		}
		else if(m_TargetIndex == 4)
		{
			m_CurSpeed = 82.0;
		}
		else if(m_TargetIndex == 5)
		{
			m_CurSpeed = 26.0;
		}
		else
		{
			m_CurSpeed = 0.0;
			m_TargetIndex = 7;
		}
	}

}

void ABladeIINetPlayer::SyncRemoteMovementState(const FVector& PreviousLocation, const FVector& NewLocation, float DeltaTime)
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		const FVector Velocity = DeltaTime > KINDA_SMALL_NUMBER
			? (NewLocation - PreviousLocation) / DeltaTime
			: FVector::ZeroVector;

		MovementComponent->Velocity = Velocity;

		const bool bHasVerticalMotion = FMath::Abs(Velocity.Z) > 10.f || FMath::Abs(m_TargetPos.Z - NewLocation.Z) > 5.f;
		MovementComponent->SetMovementMode(bHasVerticalMotion ? MOVE_Falling : MOVE_Walking);
	}
}

bool ABladeIINetPlayer::IsMoveTrace()
{
	FVector pos = GetActorLocation();
	FVector tpos = pos + GetActorForwardVector() * 60;
	pos.Z += -1000.0;
	tpos.Z += 1000.0;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	return UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), pos, tpos, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit,false);
	//static bool LineTraceSingleForObjects(const UObject * WorldContextObject, const FVector Start, const FVector End,
	//	const TArray<TEnumAsByte<EObjectTypeQuery> > &ObjectTypes, bool bTraceComplex, 
	//	const TArray<AActor*>&ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult & OutHit, bool bIgnoreSelf, 
	//	FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);
}
