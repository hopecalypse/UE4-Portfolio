// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonRoom.h"

#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Monster/AI/MonsterAIController.h"
#include "Player/PlayableCharacter.h"

void UDungeonRoom::PlayerEnter(APlayableCharacter* _Player)
{
	if(this == nullptr)
		return;
	if(bPlayerEntered || bPlayerStart)
		return;
	if(Monsters.Num() == 0)
		return;

	LOGTEXT_ERROR(TEXT("플레이어 몬스터 방(%s) 처음 입장함"), *GetName());
	bPlayerEntered = true;
	
	// 플레이어 입장-> 소속 몬스터 BT값 Trace로 설정하기
	for(int i = 0; i < Monsters.Num(); i++)
	{
		AMonsterAIController* _MonsterAIController = Cast<AMonsterAIController>(Monsters[i]->GetController());
		if(_MonsterAIController != nullptr)
		{
			_MonsterAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), _Player);
			_MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bTracingPlayer"), true);
		}
	}
}
