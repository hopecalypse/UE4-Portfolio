// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_IsPlayerBlocked.generated.h"

class AMonsterAIController;
class APlayableCharacter;
class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UService_IsPlayerBlocked : public UBTService
{
	GENERATED_BODY()

public:
	UService_IsPlayerBlocked();
	
private:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	AMonsterAIController* MonsterAIController;
	AMonsterGeneralCharacter* MonsterCharacter;
	APlayableCharacter* PlayerCharacter;
	
};
