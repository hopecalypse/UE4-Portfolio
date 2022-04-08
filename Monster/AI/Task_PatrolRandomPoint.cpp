// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_PatrolRandomPoint.h"

#include "MonsterAIController.h"
#include "NavigationSystem.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"

UTask_PatrolRandomPoint::UTask_PatrolRandomPoint()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UTask_PatrolRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//				SpawnPoint로부터 랜덤한 위치를 찾아서 이동
	// 0. 필요 컴포넌트
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	// 1. 랜덤 위치 만들기(블랙보드 Inited == false일 때)
	if(MonsterAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("PatrolInited")) == false)
	{
		// 0으로 Init
		MonsterAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolTargetLocation"), FVector::ZeroVector);
		// 정보 가져오기
		float _Radius = MonsterCharacter->GetMonsterInfo().PatrolRandomRadius;
		FVector _SpawnLocation = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("SpawnLocation"));
		// 3번 반복
		for(int i = 1; i < 4; i++)
		{
			FVector _RandomVector;
			NavigationSystem->K2_GetRandomReachablePointInRadius(MonsterCharacter, _SpawnLocation, _RandomVector, _Radius);
			MonsterAIController->GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(i)), _RandomVector);
			//LOGTEXT_ERROR(TEXT("%s"), *_RandomVector.ToString());
			// 첫 번째로 타겟 설정
			if(i == 1)
				MonsterAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolTargetLocation"), _RandomVector);
		}
		MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("PatrolInited"), true);
	}

	// 2. 다음 포인트로 이동하기(도착할 때마다 업데이트)
	MonsterCharacter->SetMovementSpeed(MonsterCharacter->GetMonsterInfo().MoveIdleSpeed);
	MonsterCharacter->SetMonsterState(EMonsterState::E_MovingIdle);
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(MonsterAIController, MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("PatrolTargetLocation")));

	// 3. Tick에서 이동 기다리기
	return EBTNodeResult::Type::InProgress;
}

void UTask_PatrolRandomPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 도착 판별
	if(MonsterAIController->GetMoveStatus() == EPathFollowingStatus::Moving)
	{
		// 이동 중
	}
	else if(MonsterAIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		// 도착함(성공)
		FVector _CurTarget = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("PatrolTargetLocation"));

		// 타겟 포인트 업데이트하기
		// 먼저 현재 목표였던 포인트의 Index 찾기
		
		int _CurIndex = 1;
		for(int i = 1; i < 4; i++)
		{
			// 만약 현재 == 포인트i이면
			if(FVector::Distance(_CurTarget, MonsterAIController->GetBlackboardComponent()->GetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(i)))) <= 10.f)
			{
				_CurIndex = i;
				break;
			}
		}

		// 현재 + 1로 타겟 설정하기
		_CurIndex += 1;
		if(_CurIndex > 3)
			_CurIndex = 1;

		// 다음 타겟을 설정하기
		FVector _NextPoint = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(_CurIndex)));
		
		MonsterAIController->GetBlackboardComponent()->SetValueAsVector(FName("PatrolTargetLocation"), _NextPoint);
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
}

void UTask_PatrolRandomPoint::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	// if(TaskResult == EBTNodeResult::Aborted)
	// 	LOGAUTO_ERROR;

	MonsterCharacter->SetMonsterState(EMonsterState::E_Idle);
	MonsterAIController->StopMovement();
}
