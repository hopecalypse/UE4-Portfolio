// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "MonsterAIController.generated.h"

class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

	// AI 필요 변수
private:
	// BlackBoard는 부모에서 정의됨
	UBehaviorTree* MonsterBehaviorTree;
	UBlackboardData* MonsterBlackboard;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

//private:
	//void AddPatrolRandomPointsToMonster(AMonsterGeneralCharacter* _MonsterCharacter, UBlackboardData* _MonsterBlackboard);

};
