// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_MoveTowardsToPlayer.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UTask_MoveTowardsToPlayer::UTask_MoveTowardsToPlayer()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTask_MoveTowardsToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 필요 오브젝트 찾기
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return EBTNodeResult::Type::Failed;
	PlayerCharacter = Cast<APlayableCharacter>(MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(PlayerCharacter == nullptr)			return EBTNodeResult::Type::Failed;
	MonsterAnimInstance = MonsterCharacter->GetMonsterAnimInstance();
	if(MonsterAnimInstance == nullptr)		return EBTNodeResult::Type::Failed;


	LOGTEXT_LOG(TEXT("Trace 시작:%s"), *MonsterCharacter->GetName());
	
	return EBTNodeResult::Type::InProgress;
}

void UTask_MoveTowardsToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 플레이어에게 이동하기
	float _AttackDistance = MonsterCharacter->GetMonsterInfo().AttackRange;
	float _Distance = FVector::Distance(MonsterCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	// 1. 이동 완료 시
	if(_Distance < _AttackDistance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
	// 2. 이동 중
	else
	{
		MonsterCharacter->SetMonsterState(EMonsterState::E_MovingTrace);
		FVector _Dir = (PlayerCharacter->GetActorLocation() - MonsterCharacter->GetActorLocation()).GetSafeNormal();
		MonsterCharacter->SetActorLocation(MonsterCharacter->GetActorLocation() + _Dir * MonsterCharacter->GetMonsterInfo().MoveTraceSpeed * DeltaSeconds * 1.f);
		FRotator _Rotator = UKismetMathLibrary::FindLookAtRotation(MonsterCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
		MonsterCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(MonsterCharacter->GetActorRotation(), _Rotator, DeltaSeconds, 300.f));
	}
}
