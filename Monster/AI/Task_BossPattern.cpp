// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_BossPattern.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/BossMonsterBase.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UTask_BossPattern::UTask_BossPattern()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UTask_BossPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	BossCharacter = Cast<ABossMonsterBase>(MonsterAIController->GetCharacter());
	Blackboard = MonsterAIController->GetBlackboardComponent();
	BossAnimInstance = BossCharacter->GetMonsterAnimInstance();
	PlayerCharacter = Cast<APlayableCharacter>(Blackboard->GetValueAsObject(TEXT("PlayerActor")));

	// 랜덤한 보스 패턴 선택하기
	//Pattern = FMath::RandRange(0, 3);
	Pattern = 0;
	BasicAtkCount = 0;
	
	return EBTNodeResult::Type::InProgress;
}

void UTask_BossPattern::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// 패턴별 구현
	// 0-> 다가가서 기본공격 3회
	if(Pattern == 0)
	{
		TickRotateTo(DeltaSeconds, PlayerCharacter->GetActorLocation());
		// 만약 보스 공격중일때
		if(BossCharacter->bIsAttacking)
		{
			return;
		}

		// 3회 공격시 종료
		if(BasicAtkCount == 3)
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);

		// 이동,회전 + 도착? 판별
		bool _bArrived = TickMoveTowward(BossCharacter->GetMonsterInfo().AttackRange, PlayerCharacter->GetActorLocation(), DeltaSeconds);
		// 도착 시 - 공격
		if(_bArrived)
		{
			LOGTEXT_ERROR(TEXT("보스 도착, 공격 시작(%d)"), BasicAtkCount);
			BossCharacter->bIsAttacking = true;
			BossAnimInstance->BossBasicAttackRand = FMath::RandRange(0, 2);
			BossCharacter->AttackType = EMonsterAttackType::E_BasicAttack;
			BossCharacter->SetMonsterState(EMonsterState::E_Attack);
			BasicAtkCount++;
		}
	}
	else if(Pattern == 1)
	{
		
	}
}

bool UTask_BossPattern::TickMoveTowward(float _ArrDistance, FVector _Target, float _DeltaTime)
{
	float _Distance = FVector::Distance(BossCharacter->GetActorLocation(), _Target);

	// 도착 시
	if(_Distance < _ArrDistance)
	{
		return true;
	}
	// 이동중
	else
	{
		BossCharacter->SetMonsterState(EMonsterState::E_MovingTrace);
		FVector _Dir = (_Target - BossCharacter->GetActorLocation()).GetSafeNormal();
		BossCharacter->SetActorLocation(BossCharacter->GetActorLocation() + _Dir * BossCharacter->GetMonsterInfo().MoveTraceSpeed * _DeltaTime * 1.f);
		return false;
	}
}

void UTask_BossPattern::TickRotateTo(float _DeltaTime, FVector _Target)
{
	FRotator _Rotator = UKismetMathLibrary::FindLookAtRotation(BossCharacter->GetActorLocation(), _Target);
	BossCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(BossCharacter->GetActorRotation(), _Rotator, _DeltaTime, 300.f));
}
