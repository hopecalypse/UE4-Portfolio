// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service_DetectPlayerDungeon.h"

UService_DetectPlayerDungeon::UService_DetectPlayerDungeon()
{
	Interval = 1.f;
	RandomDeviation = 0.f;
}

void UService_DetectPlayerDungeon::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	
}
