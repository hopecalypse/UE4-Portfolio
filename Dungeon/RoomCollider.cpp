// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/RoomCollider.h"

#include "DungeonRoom.h"
#include "PortFolio.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Monster/AI/MonsterAIController.h"
#include "Player/PlayableCharacter.h"

ARoomCollider::ARoomCollider()
{
	PrimaryActorTick.bCanEverTick = false;


	ColliderBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(ColliderBox);

	ColliderBox->SetCollisionProfileName(TEXT("OverlapPlayer"));
}

void ARoomCollider::BeginPlay()
{
	Super::BeginPlay();

	ColliderBox->OnComponentBeginOverlap.AddDynamic(this, &ARoomCollider::OnOverlapPlayer);
	ColliderBox->OnComponentEndOverlap.AddDynamic(this, &ARoomCollider::OnEndOverlapPlayer);
}

void ARoomCollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomCollider::OnOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayableCharacter* _Player = Cast<APlayableCharacter>(OtherActor);
	if(_Player != nullptr)
	{
		Room->PlayerEnter(_Player);
	}
}

void ARoomCollider::OnEndOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayableCharacter* _Player = Cast<APlayableCharacter>(OtherActor);
	if(_Player != nullptr)
	{
		LOGTEXT_ERROR(TEXT("플레이어 룸 나감(%s)"), *Room->GetName());
		if(Room != nullptr)
		{
			if(Room->GetName() == TEXT("None"))
				return;
			if(Room->bBossRoom || Room->bPlayerStart)
				return;
			if(Room->Monsters.Num() == 0)
				return;
			for(int i = 0; i < Room->Monsters.Num(); i++)
			{
				if(Room->Monsters[i] != nullptr)
				{
					AMonsterAIController* _AIC = Cast<AMonsterAIController>(Room->Monsters[i]->GetController());
					_AIC->GetBlackboardComponent()->SetValueAsBool(TEXT("bTracingPlayer"), false);
					//_AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), nullptr);
					Room->Monsters[i]->SetHPBarVisible(false);
				}
			}
		}
	}
}