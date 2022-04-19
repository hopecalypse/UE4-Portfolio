// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_MovePathsToPlayer.generated.h"

class AMonsterGeneralCharacter;
class AMonsterAIController;
class APlayableCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTask_MovePathsToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_MovePathsToPlayer();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// 필요 변수들
	APlayableCharacter* PlayerCharacter;
	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;


	TArray<FVector> PathPoints;
	FVector CurPoint;
	
};
