// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_DetectPlayerDungeon.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UService_DetectPlayerDungeon : public UBTService
{
	GENERATED_BODY()

public:
	UService_DetectPlayerDungeon();

private:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
