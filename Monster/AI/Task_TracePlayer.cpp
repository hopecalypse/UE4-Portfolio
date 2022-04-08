// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_TracePlayer.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UTask_TracePlayer::UTask_TracePlayer()
{
	bNotifyTick = 1;
	bNotifyTaskFinished = 1;
}

// 노드 진입
EBTNodeResult::Type UTask_TracePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	//////////////////////////////////////////
	// 플레이어를 향해 이동하기

	// 1. 컴포넌트 가져오기
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)		return EBTNodeResult::Type::Failed;
	PlayerCharacter = Cast<APlayableCharacter>(MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(PlayerCharacter == nullptr)			return EBTNodeResult::Type::Failed;

	// 2. Monster 상태 설정 시도
	
	// Trace 상태로 전환 실패 시 취소
	MonsterCharacter->SetMonsterState(EMonsterState::E_MovingTrace);
	if(MonsterCharacter->GetMonsterState() != EMonsterState::E_MovingTrace)		return EBTNodeResult::Failed;
	
	// 이동 속도 조절
	MonsterCharacter->SetMovementSpeed(MonsterCharacter->GetMonsterInfo().MoveTraceSpeed);
	// 이동은 TickTask에서
	
	return EBTNodeResult::Type::InProgress;
	
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
}

// 노드 Tick
void UTask_TracePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	

	// 플레이어에게 이동하기
	UAIBlueprintHelperLibrary::SimpleMoveToActor(MonsterAIController, PlayerCharacter);
	// 몬스터 - 플레이어 거리 계산
	float _Distance = FVector::Distance(MonsterCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	// 도착 시
	if(_Distance < MonsterCharacter->GetMonsterInfo().AttackRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
	// 이동 중
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::InProgress);
	}
}

// 노드 실패
EBTNodeResult::Type UTask_TracePlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	LOGTEXT_WARN(TEXT("Tast Aborted"));
	
	// 이동 중지(Player Actor 감지 벗어났을 때)
	MonsterAIController->StopMovement();
	MonsterCharacter->SetMonsterState(EMonsterState::E_Idle);
	
	return Super::AbortTask(OwnerComp, NodeMemory);
}

// 노드 종료 시 
void UTask_TracePlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	// Monster State Idle 설정
	MonsterCharacter->SetMonsterState(EMonsterState::E_Idle);
	// Trace 종료
	MonsterAIController->StopMovement();
}
