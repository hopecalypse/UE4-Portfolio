// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"


// 스태틱 변수 선언(필수)
USoundManager* USoundManager::ManagerInstance = nullptr;

void USoundManager::GenerateInst(UObject* _GameInstance)
{
	if(ManagerInstance == nullptr)
	{
		ManagerInstance = NewObject<USoundManager>(_GameInstance);
		ManagerInstance->AddToRoot();
	}
}

USoundManager* USoundManager::Instance()
{
	return ManagerInstance;
}

void USoundManager::DestroyManager()
{
	if(ManagerInstance != nullptr)
	{
		ManagerInstance->ConditionalBeginDestroy();
		ManagerInstance = nullptr;
	}
}
