// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_MoveToPatrolPoint.h"

#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Dungeon/DungeonCell.h"
#include "Kismet/KismetMathLibrary.h"
#include "Manager/PathManager.h"
#include "Monster/MonsterGeneralCharacter.h"

UTask_MoveToPatrolPoint::UTask_MoveToPatrolPoint()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTask_MoveToPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	
	UBlackboardComponent* _Blackboard = MonsterAIController->GetBlackboardComponent();

	// 초기화하기
	if(_Blackboard->GetValueAsBool(TEXT("PatrolInited")) == false)
	{
		TArray<FPathNode*> _RandPathNodes;

		FVector _SpawnLocation = _Blackboard->GetValueAsVector(TEXT("SpawnLocation"));
		FPathNode* _SpawnNode = UPathManager::Instance()->FindCloseNode(_SpawnLocation);
		// 1000만큼의 랜덤 지점 선택
		while (_RandPathNodes.Num() < 3)
		{
			FVector _RandLoc = _SpawnLocation + FVector(FMath::FRandRange(-1200.f, 1200.f), FMath::FRandRange(-1200.f, 1200.f), 0.f);
			FPathNode* _Node = UPathManager::Instance()->FindCloseNode(_RandLoc);

			if(_Node != nullptr)
			{
				if(_Node->bObstacle == false && _Node->Cell->bRoom == true && _Node->Cell->Room == _SpawnNode->Cell->Room)
				{
					_RandPathNodes.Add(_Node);
				}
			}
		}

		// 블랙보드 값으로 할당
		for(int i = 0; i < _RandPathNodes.Num(); i++)
		{
			_Blackboard->SetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(i + 1)), _RandPathNodes[i]->Location);
			if(i == 0)
				_Blackboard->SetValueAsVector(TEXT("PatrolTargetLocation"), _RandPathNodes[i]->Location);
		}

		_Blackboard->SetValueAsBool(TEXT("PatrolInited"), true);
		_Blackboard->SetValueAsInt(TEXT("PatrolIndex"), 1);
	}

	
	return EBTNodeResult::Type::InProgress;
}

void UTask_MoveToPatrolPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return;
	AMonsterGeneralCharacter* MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return;
	UBlackboardComponent* Blackboard = MonsterAIController->GetBlackboardComponent();
	if(Blackboard == nullptr)				return;
	
	FVector _CurPoint = Blackboard->GetValueAsVector(TEXT("PatrolTargetLocation"));
	FVector _MonLocation = MonsterCharacter->GetActorLocation();
	_CurPoint.Z = _MonLocation.Z;

	// 이동 완료 여부 체크
	if(FVector::Distance(_MonLocation, _CurPoint) < 10.f)
	{
		// 현재 포인트 번호 찾기
		int _CurIndex = Blackboard->GetValueAsInt(TEXT("PatrolIndex"));
		if(++_CurIndex > 3)
			_CurIndex = 1;
		FVector _NextPoint = Blackboard->GetValueAsVector(FName(TEXT("PatrolPoint") + FString::FromInt(_CurIndex)));
		Blackboard->SetValueAsVector(TEXT("PatrolTargetLocation"), _NextPoint);
		Blackboard->SetValueAsInt(TEXT("PatrolIndex"), _CurIndex);

		// 몬스터 Idle설정
		MonsterCharacter->SetMonsterState(EMonsterState::E_Idle);

		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
	
	// 현재 포인트를 향해 이동하기
	else
	{
		MonsterCharacter->SetMonsterState(EMonsterState::E_MovingIdle);
		FVector _Dir = (_CurPoint - _MonLocation).GetSafeNormal();
		MonsterCharacter->SetActorLocation(_MonLocation + (_Dir * MonsterCharacter->GetMonsterInfo().MoveIdleSpeed * 1.f * DeltaSeconds));
		
		FRotator _Rotator = UKismetMathLibrary::FindLookAtRotation(_MonLocation, _CurPoint);
		MonsterCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(MonsterCharacter->GetActorRotation(), _Rotator, DeltaSeconds, 300.f));
	}
}
