// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_AttackMelee.generated.h"

class AMonsterAIController;
class UMonsterAnimInstance;
class APlayableCharacter;
class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_AttackMelee : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Task 기본 설정
	UTask_AttackMelee();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;
	UMonsterAnimInstance* MonsterAnimInstance;
	APlayableCharacter* PlayerCharacter;

	bool bWaitStarted;
	float AttackWaitTime;
	float AttackAccTime;
	
	
};
