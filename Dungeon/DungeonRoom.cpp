// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonRoom.h"

#include "PortFolio.h"

void UDungeonRoom::PlayerEnter(APlayableCharacter* _Player)
{
	if(bPlayerEntered)
		return;
	if(Monsters.Num() == 0)
		return;
	
	// 플레이어 입장-> 소속 몬스터 BT값 Trace로 설정하기
	bPlayerEntered = true;

	LOGTEXT_LOG(TEXT("플레이어 방 입장함"));
}
