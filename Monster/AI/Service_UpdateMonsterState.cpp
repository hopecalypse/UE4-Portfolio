// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service_UpdateMonsterState.h"

#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/MonsterGeneralCharacter.h"

UService_UpdateMonsterState::UService_UpdateMonsterState()
{
	Interval = 0.25f;
	RandomDeviation = 0.f;
}

void UService_UpdateMonsterState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 몬스터 State 업데이트

	// 1. 컴포넌트 받기
	AMonsterAIController* _MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(_MonsterAIController == nullptr)				return;
	AMonsterGeneralCharacter* _MonsterCharacter = Cast<AMonsterGeneralCharacter>(_MonsterAIController->GetPawn());
	if(_MonsterCharacter == nullptr)				return;

	// 2. 블랙보드 데이터에 MonsterState 넣기
	EMonsterState _CurrentState = _MonsterCharacter->GetMonsterState();
	uint8 _EnumValue = (uint8)_CurrentState;
	_MonsterAIController->GetBlackboardComponent()->SetValueAsEnum(TEXT("MonsterState"), _EnumValue);
}
