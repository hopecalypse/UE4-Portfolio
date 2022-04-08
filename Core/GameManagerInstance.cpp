// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagerInstance.h"

#include "Dungeon/DungeonManager.h"
#include "Manager/EffectManager.h"
#include "Manager/SoundManager.h"

void UGameManagerInstance::Init()
{
	Super::Init();

	// 매니저들 Init
	UEffectManager::GenerateInst(this);
	UDungeonManager::GenerateInst(this);
}

void UGameManagerInstance::Shutdown()
{
	Super::Shutdown();

	// 매니저들 Destroy
	UEffectManager::DestroyManager();
	UDungeonManager::DestroyManager();
}

void UGameManagerInstance::SelectPlayerClass(EPlayerClass _SelectClass)
{
	SelectedPlayerClass = _SelectClass;
	HasSelectedClass = true;
}
