// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_AttackRanged.generated.h"

class APlayableCharacter;
class UMonsterAnimInstance;
class AMonsterGeneralCharacter;
class AMonsterAIController;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_AttackRanged : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_AttackRanged();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;
	UMonsterAnimInstance* MonsterAnimInstance;
	APlayableCharacter* PlayerCharacter;
	
	float AttackAccTime;
	float AttackWaitTime;
	bool bWaitStarted;
	
	
};
