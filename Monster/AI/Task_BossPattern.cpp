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

	// 변수 초기화
	ExcutedCount = 0;
	bPatternStarted = false;
	AccTime = 0.f;
	bMisc = false;
	PatternPhase = 0;

	// 랜덤한 보스 패턴 선택하기
	int _Rand = FMath::RandRange(0, 7);
	if(_Rand <= 3)
		Pattern = 0;
	else if(_Rand <= 5)
		Pattern = 1;
	else if(_Rand <= 6)
		Pattern = 2;
	else
		Pattern = 3;

	LOGTEXT_WARN(TEXT("보스 패턴 시작(%d)"), Pattern);
	
	return EBTNodeResult::Type::InProgress;
}

void UTask_BossPattern::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// 패턴별 구현
	// 0-> 다가가서 기본공격 3회
	if(Pattern == 0)
	{
		// 기본 회전
		TickRotateTo(DeltaSeconds, PlayerCharacter->GetActorLocation());
		// 실행중일때 대기
		if(bPatternStarted && BossCharacter->GetIsAttacking())
			return;

		// 이동,회전 + 도착? 판별
		if(BossCharacter->GetIsAttacking() == false)
		{
			bool _bArrived = TickMoveTowward(BossCharacter->GetMonsterInfo().AttackRange, PlayerCharacter->GetActorLocation(), DeltaSeconds);
			// 도착 시 - 공격
			if(_bArrived && !BossCharacter->GetIsAttacking())
			{
				LOGTEXT_ERROR(TEXT("보스 도착, 공격 시작(%d)"), ExcutedCount);
				BossAnimInstance->BossBasicAttackRand = FMath::RandRange(0, 2);
				BossCharacter->AttackType = EMonsterAttackType::E_BasicAttack;
				BossCharacter->SetMonsterState(EMonsterState::E_Attack);
				BossCharacter->SetIsAttacking(true);
				if(++ExcutedCount == 3)
					FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
			}
		}
	}
	// 패턴1-> 다가가서 스킬1 시전
	else if(Pattern == 1)
	{
		// 끝났는지 확인
		if(bPatternStarted && !BossCharacter->GetIsAttacking())
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
		// 도착 거리 설정
		float _ArrDistance = 800.f;
		// 기본 회전
		TickRotateTo(DeltaSeconds, PlayerCharacter->GetActorLocation());
		// 실행중일때 대기
		if(bPatternStarted && BossCharacter->GetIsAttacking())
			return;
		// 이동,회전 + 도착? 판별
		if(!BossCharacter->GetIsAttacking())
		{
			bool _bArrived = TickMoveTowward(_ArrDistance, PlayerCharacter->GetActorLocation(), DeltaSeconds);
			// 도착 시 - 공격
			if(_bArrived)
			{
				BossCharacter->SetIsAttacking(true);
				BossCharacter->AttackType = EMonsterAttackType::E_Pattern1;
				BossCharacter->SetMonsterState(EMonsterState::E_Attack);
				bPatternStarted = true;
			}
		}
	}
	// 패턴2-> 가운데로 가서 3초 기모으기, 땅 찍기
	else if(Pattern == 2)
	{
		// 끝났는지 확인
		if(bPatternStarted && !BossCharacter->GetIsAttacking())
		{
			BossAnimInstance->Montage_Stop(0.4f);
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
		}
		// 도착 거리 설정
		float _ArrDistance = 40.f;
		// 기본 회전(플레이어)
		TickRotateTo(DeltaSeconds, PlayerCharacter->GetActorLocation());
		// 실행중일때 대기
		if(bPatternStarted && BossCharacter->GetIsAttacking() && !bMisc)
		{
			AccTime += DeltaSeconds;
			if(AccTime > 3.f)
			{
				BossAnimInstance->Montage_JumpToSection(TEXT("Execute"));
				bMisc = true;
			}
			return;
		}
		// 이동,회전 + 도착? 판별(2배속도)
		bool _bArrived = TickMoveTowward(_ArrDistance, BossCharacter->SpawnLocation, DeltaSeconds, 1.5f);
		// 도착 시 - 공격
		if(_bArrived && !bMisc)
		{
			BossCharacter->SetIsAttacking(true);
			BossCharacter->AttackType = EMonsterAttackType::E_Pattern2;
			BossCharacter->PlayAnimMontage(BossCharacter->Pattern2Montage, 1.f);
			BossCharacter->SetMonsterState(EMonsterState::E_Attack);
			bPatternStarted = true;
		}
	}
	// 패턴3-> 캐스팅 후 박수 칠때마다 플레이어 자리에 번개
	else if(Pattern == 3)
	{
		//LOGTEXT_LOG(TEXT("패턴3, 페이즈%d"), PatternPhase);
		// 끝났는지 확인
		if(PatternPhase == 2 && !BossCharacter->GetIsAttacking())
		{
			BossAnimInstance->Montage_Stop(0.1f);
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
		}
		// 기본 회전(플레이어)
		TickRotateTo(DeltaSeconds, PlayerCharacter->GetActorLocation());
		// 0-> 시작
		if(PatternPhase == 0)
		{
			BossCharacter->SetIsAttacking(true);
			BossCharacter->AttackType = EMonsterAttackType::E_Pattern3;
			BossCharacter->PlayAnimMontage(BossCharacter->Pattern3Montage, 1.f);
			BossCharacter->SetMonsterState(EMonsterState::E_Attack);
			PatternPhase++;
		}
		// 1-> 캐스팅
		else if(PatternPhase == 1)
		{
			AccTime += DeltaSeconds;
			if(AccTime > 2.4f)
			{
				BossAnimInstance->Montage_JumpToSection(TEXT("Execute"), BossCharacter->Pattern3Montage);
				PatternPhase++;
			}
		}
		
	}
}

bool UTask_BossPattern::TickMoveTowward(float _ArrDistance, FVector _Target, float _DeltaTime, float _MoveMulti)
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
		BossCharacter->SetActorLocation(BossCharacter->GetActorLocation() + _Dir * BossCharacter->GetMonsterInfo().MoveTraceSpeed * _DeltaTime * _MoveMulti);
		return false;
	}
}

void UTask_BossPattern::TickRotateTo(float _DeltaTime, FVector _Target)
{
	
	FRotator _Rotator = UKismetMathLibrary::FindLookAtRotation(BossCharacter->GetActorLocation(), _Target);
	_Rotator.Pitch = 0.f;
	_Rotator.Roll = 0.f;
	BossCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(BossCharacter->GetActorRotation(), _Rotator, _DeltaTime, 300.f));
}
