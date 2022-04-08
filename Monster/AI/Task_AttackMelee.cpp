// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_AttackMelee.h"

#include "AIController.h"
#include "MonsterAIController.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monster/MonsterAnimInstance.h"
#include "Player/PlayableCharacter.h"

UTask_AttackMelee::UTask_AttackMelee()
{
	bNotifyTick = 1;
	bNotifyTaskFinished = 0;

	AttackAccTime = 0.f;
	AttackWaitTime = 0.f;
}

EBTNodeResult::Type UTask_AttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 필요 오브젝트 찾기
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return EBTNodeResult::Type::Failed;
	PlayerCharacter = Cast<APlayableCharacter>(MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(PlayerCharacter == nullptr)			return EBTNodeResult::Type::Failed;
	MonsterAnimInstance = MonsterCharacter->GetMonsterAnimInstance();
	if(MonsterAnimInstance == nullptr)		return EBTNodeResult::Type::Failed;

	// 몬스터 공격 State 설정
	MonsterCharacter->SetMonsterState(EMonsterState::E_Attack);
	// 설정 실패시 Task 실패 반환
	if(MonsterCharacter->GetMonsterState() != EMonsterState::E_Attack)		return EBTNodeResult::Type::Failed;

	// 몬스터 Attack Info 받아오기(Wait을 위해)
	AttackAccTime = 0.f;
	bWaitStarted = false;
	AttackWaitTime = MonsterCharacter->GetMonsterAttackInfo().WaitAfter;

	return EBTNodeResult::Type::InProgress;
}

void UTask_AttackMelee::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(bWaitStarted)
	{
		AttackAccTime += DeltaSeconds;
	}

	if(bWaitStarted && AttackAccTime >= AttackWaitTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	// 몬스터 Attack 애니메이션 실행중
	if(MonsterCharacter->GetMonsterState() == EMonsterState::E_Attack)
	{
		
	}
	// 몬스터 Attack 애니메이션 끝 -> Wait 시작
	else
	{
		bWaitStarted = true;
	}
	
	// 플레이어 방향으로 회전
	// 플레이어를 바라보도록 만들기
	AMonsterAIController* _MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(_MonsterAIController == nullptr)			return;
	AMonsterGeneralCharacter* _MonsterCharacter = Cast<AMonsterGeneralCharacter>(_MonsterAIController->GetPawn());
	if(_MonsterCharacter == nullptr)			return;
	APlayableCharacter* _PlayerCharacter = Cast<APlayableCharacter>(_MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(_PlayerCharacter == nullptr)				return;

	FRotator _ToPlayerRot = UKismetMathLibrary::FindLookAtRotation(_MonsterCharacter->GetActorLocation(), _PlayerCharacter->GetActorLocation());

	_MonsterCharacter->K2_SetActorRotation(_ToPlayerRot, false);
}


