// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_PlayerWithinDistance.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UService_PlayerWithinDistance : public UBTService
{
	GENERATED_BODY()

public:
	// Tick 초기 설정을 위한 생성자
	UService_PlayerWithinDistance();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
