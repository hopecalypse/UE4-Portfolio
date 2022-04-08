// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectManager.h"

#include "PortFolio.h"
#include "Combat/Player/PoseEffectActor.h"
#include "GameFramework/Character.h"

UEffectManager* UEffectManager::ManagerInstance = nullptr;

UEffectManager::UEffectManager()
{
	bIsOnRunning = GIsRunning;
}

void UEffectManager::GenerateInst(UObject* _GameInstance)
{
	if(ManagerInstance != nullptr)
		return;
		
	ManagerInstance = NewObject<UEffectManager>(_GameInstance);
	ManagerInstance->AddToRoot();
}

UEffectManager* UEffectManager::Instance()
{
	return ManagerInstance;
}

void UEffectManager::DestroyManager()
{
	if(ManagerInstance == nullptr)
		return;

	ManagerInstance->ConditionalBeginDestroy();
}

void UEffectManager::Tick(float DeltaTime)
{
	TickPoseEffectLoop(DeltaTime);
}

bool UEffectManager::IsTickable() const
{
	return bIsOnRunning;
}

bool UEffectManager::IsTickableInEditor() const
{
	return false;
}

bool UEffectManager::IsTickableWhenPaused() const
{
	return false;
}

TStatId UEffectManager::GetStatId() const
{
	return UObject::GetStatID();
}

void UEffectManager::PoseEffectOnce(ACharacter* _Character, float _LastTime)
{
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = _Character->GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = true;

	APoseEffectActor* _GenPoseActor = _Character->GetWorld()->SpawnActor<APoseEffectActor>(APoseEffectActor::StaticClass(), _Character->GetMesh()->GetComponentLocation(), _Character->GetMesh()->GetComponentRotation(), _SpawnParams);
	_GenPoseActor->InitPoseEffect(_Character, _LastTime);
	_GenPoseActor->FinishSpawning(_GenPoseActor->GetTransform());
}

void UEffectManager::SetPoseEffectLoop(bool _Value, ACharacter* _Character, float _Interval, float _LastTime)
{
	// 효과 시작 설정
	if(_Value)
	{
		bIsOnPoseLoop = true;
		PoseLoopInterval = _Interval;
		PoseLoopLastTime = _LastTime;
		PoseLoopCharacter = _Character;
	}
	//효과 종료
	else
	{
		bIsOnPoseLoop = false;
		PoseLoopCharacter = nullptr;
		PoseLoopLastTime = 0.f;
		PoseLoopAccTime = 0.f;
		PoseLoopInterval = 0.f;
	}
}

void UEffectManager::TickPoseEffectLoop(float _DeltaTime)
{
	if(bIsOnPoseLoop)
	{
		PoseLoopAccTime += _DeltaTime;
		if(PoseLoopAccTime > PoseLoopInterval)
		{
			PoseLoopAccTime = 0.f;
			PoseEffectOnce(PoseLoopCharacter, PoseLoopLastTime);
		}
	}
}

