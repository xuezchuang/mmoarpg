#pragma once

#include "CoreMinimal.h"
#include "BladeIICharacter.h"
#include "InputActionValue.h"
#include "BladeIINetCharacter.generated.h"

class FSimpleChannel;
struct S_MOVE_ROLE;
struct FMMOARPGUserData;

UCLASS(config=Game)
class ABladeIINetCharacter : public ABladeIICharacter
{
	GENERATED_BODY()

public:
	ABladeIINetCharacter();

	virtual void BeginPlay()	override;

	void SubscribeEvents();
	void UnsubscribeEvents();

	void UpdateLocation(const S_MOVE_ROLE* rMove);

protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	void UpdateSyncedMove();
	void SendSynecdMove(int kind,int state);
	bool IsMoveTrace();
private:
	FMMOARPGUserData* m_UserData;
	FVector m_SyncedPos;
	float m_SynedTime;
	float m_SynedTimeTemp;
	float m_face = 0;
	int m_State = 0;
};

