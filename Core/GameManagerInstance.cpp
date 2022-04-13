// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagerInstance.h"

#include "CustomEnums.h"
#include "Dungeon/DungeonManager.h"
#include "Manager/EffectManager.h"
#include "Manager/PathManager.h"
#include "Manager/SoundManager.h"

UGameManagerInstance::UGameManagerInstance()
{
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _WarriorBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerWarrior.BP_PlayerWarrior_C'"));
	if(_WarriorBP.Succeeded())
		WarriorBP = _WarriorBP.Class;
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _MageBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerMage.BP_PlayerMage_C'"));
	if(_MageBP.Succeeded())
		MageBP = _MageBP.Class;
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _GunnerBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerGunner.BP_PlayerGunner_C'"));
	if(_GunnerBP.Succeeded())
		GunnerBP = _GunnerBP.Class;
}

void UGameManagerInstance::Init()
{
	Super::Init();

	// 매니저들 Init
	UEffectManager::GenerateInst(this);
	UDungeonManager::GenerateInst(this);
	UPathManager::GenerateInst(this);
}

void UGameManagerInstance::Shutdown()
{
	Super::Shutdown();

	// 매니저들 Destroy
	UEffectManager::DestroyManager();
	UDungeonManager::DestroyManager();
	UPathManager::DestroyManager();
}

void UGameManagerInstance::SelectPlayerClass(EPlayerClass _SelectClass)
{
	SelectedPlayerClass = _SelectClass;
	HasSelectedClass = true;
}

TSubclassOf<APlayableCharacter> UGameManagerInstance::GetSelectedClassBP()
{
	if(SelectedPlayerClass == EPlayerClass::E_Warrior)
		return WarriorBP;
	else if(SelectedPlayerClass == EPlayerClass::E_Mage)
		return MageBP;
	else
		return GunnerBP;
}
