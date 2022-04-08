// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/MonsterAIController.h"

#include "DrawDebugHelpers.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "NavigationSystem.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"

AMonsterAIController::AMonsterAIController()
{
	MonsterBehaviorTree = nullptr;
	MonsterBlackboard = nullptr;
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 컨트롤 중인 몬스터 캐릭터(Pawn) 가져오기
	AMonsterGeneralCharacter* _MonsterCharacter = Cast<AMonsterGeneralCharacter>(InPawn);
	if (_MonsterCharacter != nullptr)
	{
		// BlackBoard, BT 가져오기
		MonsterBehaviorTree = _MonsterCharacter->GetBehaviorTree();
		MonsterBlackboard = _MonsterCharacter->GetBlackboardData();

		// 블랙보드 사용 설정
		UseBlackboard(MonsterBlackboard, Blackboard);
		// BT 시작하기
		RunBehaviorTree(MonsterBehaviorTree);
		
		// 스폰 위치 할당
		if(GetBlackboardComponent() != nullptr)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("SpawnLocation"), _MonsterCharacter->GetActorLocation());
		}
	}

}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();


}