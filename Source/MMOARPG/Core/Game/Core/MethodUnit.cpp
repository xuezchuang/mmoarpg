#include "MethodUnit.h"

namespace MethodUnit
{
	template<class T>
	void ServerCallAllPlayerController(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		for (FConstPlayerControllerIterator It = NewWorld->GetPlayerControllerIterator(); It; ++It)
		{
			if (T* InPlayerController = Cast<T>(It->Get()))
			{
				if (InImplement(InPlayerController) == EServerCallType::PROGRESS_COMPLETE)
				{
					break;
				}
			}
		}
	}

	template<class T>
	void ServerCallAllPlayer(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		ServerCallAllPlayerController<APlayerController>(NewWorld, [&](APlayerController* InPlayerContoroller)->EServerCallType
		{
			if (T* InPawn = Cast<T>(InPlayerContoroller->GetPawn()))
			{
				return InImplement(InPawn);
			}

			return EServerCallType::INPROGRESS;
		});
	}
}