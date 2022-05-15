// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonRoom.h"

#include "DungeonCell.h"
#include "DungeonDataAsset.h"
#include "DungeonManager.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/RectLightComponent.h"
#include "Engine/RectLight.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Monster/AI/MonsterAIController.h"
#include "Player/PlayableCharacter.h"
#include "UI/PlayerHUD.h"

void UDungeonRoom::PlayerEnter(APlayableCharacter* _Player)
{
	if(this == nullptr)
	{
		LOGAUTO_LOG;
		LOGAUTO_ERROR;
		return;
	}
	if(this->bPlayerStart)
		return;
	// if(this->bPlayerEntered)
	// 	return;

	//LOGTEXT_ERROR(TEXT("플레이어 몬스터 방(%s) 처음 입장함"), *GetName());
	//bPlayerEntered = true;
	/*if(GetName() == TEXT("None"))
		return;*/

	if(this->Monsters.Num() == 0)
		return;

	if(!bBossRoom)
	{
		// 플레이어 입장-> 소속 몬스터 BT값 Trace로 설정하기
		for(int i = 0; i < Monsters.Num(); i++)
		{
			if(Monsters[i] == nullptr)
				continue;
			AMonsterAIController* _MonsterAIController = Cast<AMonsterAIController>(Monsters[i]->GetController());
			if(_MonsterAIController != nullptr)
			{
				_MonsterAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), _Player);
				_MonsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bTracingPlayer"), true);
			}
			Monsters[i]->SetHPBarVisible(true);
		}
	}
	else
	{
		AMonsterAIController* _MonsterAIController = Cast<AMonsterAIController>(Monsters[0]->GetController());
		if(_MonsterAIController != nullptr)
		{
			_MonsterAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), _Player);
			UDungeonManager::Instance()->PlayerHUD->GetWidgetFromName(TEXT("BossPanel"))->SetVisibility(ESlateVisibility::Visible);
		}
		
	}
}

void UDungeonRoom::GenerateLevel()
{
	// 항상: Center Cell에 Lamp 생성하기
	FActorSpawnParameters _SpawnParams;
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();

	// Ceiling Lamp 메시 액터
	// TSubclassOf<AActor> _CeilingLightClass = UDungeonManager::Instance()->GetLevelData()->RoomCeilingLight;
	// AActor* _CeilingLight = GetOuter()->GetWorld()->SpawnActor<AActor>(_CeilingLightClass, CenterCell->Location + FVector(0.f, 0.f, 500.f), FRotator::ZeroRotator, _SpawnParams);
	// CenterCell->PropActors.Add(TEXT("CeilingLight"), _CeilingLight);

	// Ceiling Rect 라이트
	// AActor* _RectLight = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->RoomRectLight, CenterCell->Location + FVector(0.f, 0.f, 600.f), FRotator::ZeroRotator, _SpawnParams);
	// URectLightComponent* _LightComp = _RectLight->FindComponentByClass<URectLightComponent>();
	//
	// float _Width = Rect->Width * 600.f;
	// float _Height = Rect->Height * 600.f;
	// _LightComp->SetSourceWidth(_Width);
	// _LightComp->SetSourceHeight(_Height);
}
