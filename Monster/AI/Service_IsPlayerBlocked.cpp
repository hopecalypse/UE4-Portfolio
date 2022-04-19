// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service_IsPlayerBlocked.h"

#include "MonsterAIController.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"

UService_IsPlayerBlocked::UService_IsPlayerBlocked()
{
	Interval = 0.5f;
	RandomDeviation = 0.f;
	bNotifyTick = true;
	bRestartTimerOnEachActivation = false;
	bCreateNodeInstance = true;
}

void UService_IsPlayerBlocked::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	// 기본 변수 찾기
	// 필요 오브젝트 찾기
	MonsterAIController = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	if(MonsterAIController == nullptr)		return;
	MonsterCharacter = Cast<AMonsterGeneralCharacter>(MonsterAIController->GetPawn());
	if(MonsterCharacter == nullptr)			return;
	PlayerCharacter = Cast<APlayableCharacter>(MonsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if(PlayerCharacter == nullptr)			return;
	
	// 플레이어 감지, 벽에 막혀있으면 A*로, 아니면 그냥 직전
	TArray<AActor*> _ActorsToIgnore;
	FHitResult _HitResult;
	bool bResult = UKismetSystemLibrary::LineTraceSingle(MonsterCharacter, MonsterCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation(), UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9), false, _ActorsToIgnore, EDrawDebugTrace::None, _HitResult, true);

	if(bResult)
	{
		// 플레이어 Hit
		if(Cast<APlayableCharacter>(_HitResult.Actor) != nullptr)
		{
			MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bPlayerBlocked"), false);
			//LOGTEXT_LOG(TEXT("플레이어(%s) 감지(%s)"),*_HitResult.Actor->GetName(), *MonsterAIController->GetName());
			return;
		}
	}
	MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bPlayerBlocked"), true);
	//LOGTEXT_LOG(TEXT("플레이어 막힘(%s)"), *MonsterAIController->GetName());
}
