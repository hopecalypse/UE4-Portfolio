// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service_RotateToPlayer.h"

#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UService_RotateToPlayer::UService_RotateToPlayer()
{
	Interval = 1.f;
	RandomDeviation = 0.f;
}

void UService_RotateToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

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
