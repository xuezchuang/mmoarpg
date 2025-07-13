// Copyright Epic Games, Inc. All Rights Reserved.

#include "BladeIINetCharacter.h"
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
#include "Kismet/GameplayStatics.h"
#include "B2NetGameMode.h"
//////////////////////////////////////////////////////////////////////////
// ABladeIINetCharacter

ABladeIINetCharacter::ABladeIINetCharacter()
{
	
}

void ABladeIINetCharacter::BeginPlay()
{
	Super::BeginPlay();
	m_UserData = &GetGameInstance<UMMOARPGGameInstance>()->GetUserData();
	UE_LOG(MMOARPG, Display, TEXT("Init Pos [%d,%d,%d]"), m_UserData->base.status.pos.x, m_UserData->base.status.pos.y, m_UserData->base.status.pos.z);
	m_SyncedPos.X = m_UserData->base.status.pos.x;
	m_SyncedPos.Y = m_UserData->base.status.pos.y;
	m_SyncedPos.Z = m_UserData->base.status.pos.z;
	SetActorLocation(m_SyncedPos);
}

void ABladeIINetCharacter::SubscribeEvents()
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

void ABladeIINetCharacter::UnsubscribeEvents()
{

}

void ABladeIINetCharacter::UpdateLocation(const S_MOVE_ROLE* rMove)
{
	//FVector NewLocation;
	//	
	//NewLocation.X = rMove->face
	//	, FRotator NewRotation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport
	//SetActorLocationAndRotation(rMove.face)
	//	return SetActorLocationAndRotation(NewLocation, NewRotation, bSweep, (bSweep ? &SweepHitResult : nullptr), TeleportFlagToEnum(bTeleport));

}

void ABladeIINetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(UGameplayStatics::GetGameMode(GetWorld())->IsA<ABladeIINetGameMode>())
	{
		m_CurSpeed = GetVelocity().Size();
		UpdateSyncedMove();
	}
}

void ABladeIINetCharacter::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	//switch(ProtocolNumber)
	//{
	//case SP_UpdateOtherPos:
	//{

	//	break;
	//}
	//}
}

void ABladeIINetCharacter::UpdateSyncedMove()
{
	//1 step
	int state = 0;
	if(m_CurSpeed > 0.0)
	{
		state = 1;
	}
	if(m_State == 0)
	{
		m_SynedTime = GetWorld()->GetTimeSeconds();
	}
	//2 step
	if(state != m_State)
	{
		SendSynecdMove(0, state);
		return;
	}
	//
	if(GetWorld()->GetTimeSeconds() - m_SynedTime > 0.1)
	{
		//3 step
		if(abs(GetActorRotation().Yaw - m_face) > 1)
		{
			SendSynecdMove(1, state);
			return;
		}
		//4 step
		if(m_CurSpeed > 0 && (m_SyncedPos-GetActorLocation()).Length() > 5.0)
		{
			SendSynecdMove(1, state);
			return;
		}
	}
}

void ABladeIINetCharacter::SendSynecdMove(int kind, int state)
{
	m_State = state;
	float speed;
	FVector targetpos;
	//UE_LOG(MMOARPG, Display, TEXT("FVector sizeof:%d"), sizeof(FVector));
	m_face = GetActorRotation().Yaw;
	m_SyncedPos = GetActorLocation();
	speed = m_CurSpeed;
	m_SynedTime = GetWorld()->GetTimeSeconds();
	//1 step
	float value = 60.0;
	if(GetWorld()->GetTimeSeconds() - m_SynedTimeTemp < 0.25)
		value = 30.0;
	//2 step
	
	if(speed >0)
	{
		targetpos = m_SyncedPos + GetActorForwardVector() * value;
	}
	else
	{
		targetpos = m_SyncedPos;
	}
	//3 step
	if(IsMoveTrace())
	{
		targetpos = GetActorLocation();
	}
	else if(kind > 0)
	{
		speed = 600.0;
	}
	//4 step
	int16 tface = m_face * 100;
	int32 tspeed = speed * 100;
	S_VECTOR3 s_pos, s_tpos;
	s_pos.x = m_SyncedPos.X;
	s_pos.y = m_SyncedPos.Y;
	s_pos.z = m_SyncedPos.Z;
	s_tpos.x = targetpos.X;
	s_tpos.y = targetpos.Y;
	s_tpos.z = targetpos.Z;

	SEND_DATA(SP_UpdatePos, tface, tspeed, s_pos, s_tpos);
	UE_LOG(MMOARPG, Display, TEXT("CurPos:%d-%d-%d"), s_pos.x, s_pos.y,s_pos.z);
}

bool ABladeIINetCharacter::IsMoveTrace()
{
	FVector pos = GetActorLocation();
	FVector tpos = pos + GetActorForwardVector() * 60;
	pos.Z += -1000.0;
	tpos.Z += 1000.0;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	//ObjectTypes.Add(EObjectTypeQuery::wo)
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	return UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), pos, tpos, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit,false);
	//static bool LineTraceSingleForObjects(const UObject * WorldContextObject, const FVector Start, const FVector End,
	//	const TArray<TEnumAsByte<EObjectTypeQuery> > &ObjectTypes, bool bTraceComplex, 
	//	const TArray<AActor*>&ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult & OutHit, bool bIgnoreSelf, 
	//	FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);
}
