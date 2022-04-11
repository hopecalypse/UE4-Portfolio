// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PathManager.h"

UPathManager* UPathManager::ManagerInstance = nullptr;

void UPathManager::GenerateInst(UObject* _GameInstance)
{
	if(ManagerInstance == nullptr)
	{
		ManagerInstance = NewObject<UPathManager>(_GameInstance);
		ManagerInstance->AddToRoot();
	}
}

UPathManager* UPathManager::Instance()
{
	return ManagerInstance;
}

void UPathManager::DestroyManager()
{
	if(ManagerInstance != nullptr)
	{
		ManagerInstance->ConditionalBeginDestroy();
		ManagerInstance = nullptr;
	}
}
