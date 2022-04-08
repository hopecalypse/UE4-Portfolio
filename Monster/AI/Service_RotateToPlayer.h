// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_RotateToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UService_RotateToPlayer : public UBTService
{
	GENERATED_BODY()


public:
	UService_RotateToPlayer();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
