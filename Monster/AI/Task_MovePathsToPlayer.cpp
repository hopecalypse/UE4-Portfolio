// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task_MovePathsToPlayer.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/PathManager.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UTask_MovePathsToPlayer::UTask_MovePathsToPlayer()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTask_MovePathsToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return EBTNodeResult::Type::Failed;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return EBTNodeResult::Type::Failed;
	PlayerCharacter = Cast<APlayableCharacter>(MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(PlayerCharacter == nullptr)			return EBTNodeResult::Type::Failed;

	// A* 경로 탐색
	PathPoints = UPathManager::Instance()->FindPath(MonsterCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	CurPoint = PathPoints.Pop();
	UKismetSystemLibrary::DrawDebugSphere(MonsterCharacter, CurPoint, 50.f, 10.f, FLinearColor::Blue, 10.f, 10.f);
	
	return EBTNodeResult::Type::InProgress;
}

void UTask_MovePathsToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 경로대로 움직이기
	
	// 거리 검사
	float _CurDistance = FVector::Distance(CurPoint, MonsterCharacter->GetActorLocation());
	// 도착 시
	if(_CurDistance < 100.f)
	{
		LOGTEXT_WARN(TEXT("추격 패스 이동 완료"));
		if(PathPoints.Num() > 0)
			CurPoint = PathPoints.Pop();
		else
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
	}
	// 이동
	else
	{
		LOGTEXT_LOG(TEXT("추격 패스 이동중(%d)"), PathPoints.Num());
		MonsterCharacter->SetMonsterState(EMonsterState::E_MovingTrace);
		
		FVector _MonsterLoc = MonsterCharacter->GetActorLocation();
		CurPoint.Z = _MonsterLoc.Z;
		FVector _Dir = (CurPoint - _MonsterLoc).GetSafeNormal();
		MonsterCharacter->SetActorLocation(_MonsterLoc + (_Dir * MonsterCharacter->GetMonsterInfo().MoveTraceSpeed * 1.f * DeltaSeconds));
		
		FRotator _Rotator = UKismetMathLibrary::FindLookAtRotation(_MonsterLoc, CurPoint);
		MonsterCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(MonsterCharacter->GetActorRotation(), _Rotator, DeltaSeconds, 300.f));
	}
}
