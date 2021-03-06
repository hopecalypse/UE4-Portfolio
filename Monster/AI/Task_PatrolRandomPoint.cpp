// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_PatrolRandomPoint.h"

#include "MonsterAIController.h"
#include "NavigationSystem.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Dungeon/DungeonCell.h"
#include "Dungeon/DungeonRoom.h"
#include "Manager/PathManager.h"
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
		// 목표 위치값 초기화
		MonsterAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolTargetLocation"), FVector::ZeroVector);
		// 정보 가져오기
		FVector _SpawnLocation = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("SpawnLocation"));
		_SpawnLocation.Z = 0;
		
		//! Renew -> 랜덤 지점 찍기
		FPathNode* _SpawnNode = UPathManager::Instance()->FindCloseNode(_SpawnLocation);
		TArray<FPathNode*> _RandNodes;
		while (_RandNodes.Num() < 3)
		{
			FVector2D _RandVec = FVector2D(FMath::RandRange(-10, 10), FMath::RandRange(-10, 10));
			FPathNode* _Node = UPathManager::Instance()->PathNodeMap.FindRef(_SpawnNode->Matrix + _RandVec);
			if(_Node != nullptr)
				_RandNodes.AddUnique(_Node);
		}

		// 3개의 노드 BT에 등록하기
		for(int i = 0; i < _RandNodes.Num(); i++)
		{
			MonsterAIController->GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(i + 1)), _RandNodes[i]->Location);
			// 첫번째 포인트를 등록
			if(i == 0)
				MonsterAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolTargetLocation"), _RandNodes[i]->Location);
		}

		// Init 설정
		MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("PatrolInited"), true);
	}
	
	// 2. 이동시키기
	MonsterCharacter->SetDestination(MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("PatrolTargetLocation")));
	MonsterCharacter->SetMonsterState(EMonsterState::E_MovingIdle);
	
	// 2. Tick에서 이동하기
	return EBTNodeResult::Type::InProgress;
	
	// 2. 다음 포인트로 이동하기(도착할 때마다 업데이트)
	// MonsterCharacter->SetMovementSpeed(MonsterCharacter->GetMonsterInfo().MoveIdleSpeed);
	// MonsterCharacter->SetMonsterState(EMonsterState::E_MovingIdle);
	// UAIBlueprintHelperLibrary::SimpleMoveToLocation(MonsterAIController, MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("PatrolTargetLocation")));
}

void UTask_PatrolRandomPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// FVector _CurLocation = MonsterCharacter->GetActorLocation();
	// _CurLocation.Z = 0;
	FVector _CurTarget = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(TEXT("PatrolTargetLocation"));

	// 1. 도착했는지 검사하기
	if(!MonsterCharacter->bPathMoving)
	{
		LOGTEXT_LOG(TEXT("도착 완료!"));
		// 도착 완료->현재 index 찾고 다음 포인트 설정하기
		int _CurIdx = 1;
		for(int i = 1; i < 4; i++)
		{
			if(FVector::Distance(_CurTarget, MonsterAIController->GetBlackboardComponent()->GetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(i)))) < 10.f)
			{
				_CurIdx = i;
				break;
			}
		}

		// 3이상이면 1로 돌아오기
		if(_CurIdx++ > 3)
			_CurIdx = 1;

		// 목표 업데이트하기
		FVector _NextTarget = MonsterAIController->GetBlackboardComponent()->GetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(_CurIdx)));
		MonsterAIController->GetBlackboardComponent()->SetValueAsVector(FName("PatrolTargetLocation"), _NextTarget);

		// 노드 종료
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}

	// 2. 그 위치로 이동시키기
	
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
