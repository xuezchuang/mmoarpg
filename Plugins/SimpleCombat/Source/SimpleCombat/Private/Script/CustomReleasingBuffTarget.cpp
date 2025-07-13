//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Script/CustomReleasingBuffTarget.h"
#include "Kismet/GameplayStatics.h"

void UCustomReleasingBuffTarget::GetCustomReleasingBuffTarget(AActor * InOwner, TArray<AActor*>&OutTarget)
{
	K2_GetCustomReleasingBuffTarget(InOwner, OutTarget);
}

UWorld* UCustomReleasingBuffTarget::GetWorld2(AActor* InOwner)
{
	return InOwner->GetWorld();
}

bool UCustomReleasingBuffTarget::GetAllActorsOfClass(AActor* InOwner, TSubclassOf<AActor> ActorClass, TArray<AActor*>& InArray)
{
	UGameplayStatics::GetAllActorsOfClass(InOwner->GetWorld(), ActorClass, InArray);

	return InArray.Num()> 0;
}
