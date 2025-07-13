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
	ABladeIINetPlayer();

	virtual void BeginPlay()	override;

	void SubscribeEvents();
	void UnsubscribeEvents();

	void UpdateMoveData(const S_MOVE_ROLE* rMove);
	void UpdateBaseData(const S_ROLE_O_BASE* RoleBase);
protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);
	void UpDateMove(float DeltaTime);
	bool IsMoveTrace();
private:
	FVector m_TargetPos;
	FVector m_CurPos;
	float m_CurFace;
	float m_TargetFace;
	float m_TargetSpeedTemp=0.0;
	int32 m_TargetIndex = 0;
};

