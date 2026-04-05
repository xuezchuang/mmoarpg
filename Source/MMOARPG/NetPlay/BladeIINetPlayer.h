#pragma once

#include "CoreMinimal.h"
#include "BladeIIPlayer.h"
#include "InputActionValue.h"
#include "BladeIINetPlayer.generated.h"

class FSimpleChannel;
struct S_MOVE_ROLE;
struct S_ROLE_O_BASE;

UCLASS()
class ABladeIINetPlayer : public ABladeIIPlayer
{
	GENERATED_BODY()

public:
	ABladeIINetPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()	override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SubscribeEvents();
	void UnsubscribeEvents();

	void UpdateMoveData(const S_MOVE_ROLE* rMove);
	void UpdateBaseData(const S_ROLE_O_BASE* RoleBase);
	void SetRemotePlayerId(uint32 InRemotePlayerId);
	uint32 GetRemotePlayerId() const;



protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);
	void UpDateMove(float DeltaTime);
	bool IsMoveTrace();
	void SyncRemoteMovementState(const FVector& PreviousLocation, const FVector& NewLocation, float DeltaTime);
private:
	FVector m_TargetPos;
	FVector m_CurPos;
	float m_CurFace;
	float m_TargetFace;
	float m_TargetSpeedTemp=0.0;
	int32 m_TargetIndex = 0;
	uint32 RemotePlayerId = MAX_uint32;
};

