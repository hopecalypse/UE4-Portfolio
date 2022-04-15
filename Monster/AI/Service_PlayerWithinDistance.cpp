// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service_PlayerWithinDistance.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UService_PlayerWithinDistance::UService_PlayerWithinDistance()
{
	Interval = 1.f;

	RandomDeviation = 0.f;

	//NodeName = TEXT("asd")
}

void UService_PlayerWithinDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	////////////////////////////////////////////
	// 플레이어 캐릭터가 일정 거리 안에 있는지 확인하기

	// 1. 필요 오브젝트들 찾기
	AAIController* _MonsterAIC = OwnerComp.GetAIOwner();
	if(_MonsterAIC == nullptr)		return;
	AMonsterGeneralCharacter* _MonsterCharacter = Cast<AMonsterGeneralCharacter>(_MonsterAIC->GetPawn());
	if(_MonsterCharacter == nullptr)		return;
	// 플레이어 캐릭터 찾기(PlayerController 찾은 후 GetPawn)
	APlayableCharacter* _PlayerCharacter = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if(_PlayerCharacter == nullptr)			return;

	// 2. 몬스터와 캐럭터의 Location 거리 검사
	float _DetectRange = _MonsterCharacter->GetMonsterInfo().DetectRange;

	float _Distance = FVector::Distance(_MonsterCharacter->GetActorLocation(), _PlayerCharacter->GetActorLocation());

	// 탐지 범위 안에 있으면
	if(_Distance < _DetectRange)
	{
		// 플레이어를 블랙보드 값에 설정하기
		_MonsterAIC->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), _PlayerCharacter);
	}
	else
	{
		// 블랙보드 값 null
		_MonsterAIC->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), nullptr);
	}

// #ifdef ENABLE_DRAW_DEBUG
// 	DrawDebugSphere(GetWorld(), _MonsterCharacter->GetActorLocation(), _DetectRange, 40,
// 		_Distance < _DetectRange ? FColor::Red : FColor::White,
// 		false, 0.5f
// 		);
// #endif
}
