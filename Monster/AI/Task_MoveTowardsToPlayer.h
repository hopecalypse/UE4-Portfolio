// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_MoveTowardsToPlayer.generated.h"

class UMonsterAnimInstance;
class APlayableCharacter;
class AMonsterGeneralCharacter;
class AMonsterAIController;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_MoveTowardsToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_MoveTowardsToPlayer();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;
	APlayableCharacter* PlayerCharacter;
	UMonsterAnimInstance* MonsterAnimInstance;
	
};
