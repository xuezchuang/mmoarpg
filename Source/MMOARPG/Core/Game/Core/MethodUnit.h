
#pragma once

#include "CoreMinimal.h"

namespace MethodUnit
{
	enum EServerCallType :uint8
	{
		INPROGRESS,
		PROGRESS_COMPLETE
	};

	template<class T>
	void ServerCallAllPlayerController(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);
	
	template<class T>
	void ServerCallAllPlayer(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);
}