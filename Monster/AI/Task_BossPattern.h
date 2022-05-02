// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_BossPattern.generated.h"

class APlayableCharacter;
class UMonsterAnimInstance;
class ABossMonsterBase;
class AMonsterAIController;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_BossPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_BossPattern();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	bool TickMoveTowward(float _ArrDistance, FVector _Target, float _DeltaTime, float _MoveMulti = 1.f);
	void TickRotateTo(float _DeltaTime, FVector _Target);

	AMonsterAIController* MonsterAIController;
	ABossMonsterBase* BossCharacter;
	UBlackboardComponent* Blackboard;
	UMonsterAnimInstance* BossAnimInstance;
	APlayableCharacter* PlayerCharacter;
	int32 Pattern;
	int32 ExcutedCount;
	bool bPatternStarted;
	bool bMisc;
	int32 PatternPhase;
	float AccTime;
	
};
