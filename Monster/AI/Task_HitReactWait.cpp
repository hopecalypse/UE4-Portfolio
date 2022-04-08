// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_HitReactWait.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "Monster/MonsterGeneralCharacter.h"

UTask_HitReactWait::UTask_HitReactWait()
{
	bNotifyTick = 1;
	bNotifyTaskFinished = 0;
}

EBTNodeResult::Type UTask_HitReactWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 필요 오브젝트 찾기
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return EBTNodeResult::Type::Failed;

	if(MonsterCharacter->GetMonsterState() != EMonsterState::E_HitReact)
		return EBTNodeResult::Type::Failed;
	else
		return EBTNodeResult::Type::InProgress;
}

void UTask_HitReactWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(MonsterCharacter->GetMonsterState() == EMonsterState::E_HitReact)
	{
		//LOGAUTO_WARN;
	}
	else
	{
		//LOGAUTO_ERROR;
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
}
