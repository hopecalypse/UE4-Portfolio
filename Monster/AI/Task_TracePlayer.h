// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_TracePlayer.generated.h"

class AMonsterGeneralCharacter;
class AMonsterAIController;
class APlayableCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_TracePlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Tick, Notify 설정하기
	UTask_TracePlayer();

protected:
	// BT 노드 기본 함수들
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	// 필요 변수들
	APlayableCharacter* PlayerCharacter;
	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;
	
	
};
