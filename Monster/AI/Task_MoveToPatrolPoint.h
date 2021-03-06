// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_MoveToPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_MoveToPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_MoveToPatrolPoint();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
