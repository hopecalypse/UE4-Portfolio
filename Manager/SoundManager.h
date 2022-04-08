// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API USoundManager : public UObject
{
	GENERATED_BODY()
	
private:
	static USoundManager* ManagerInstance;
	
public:
	static void GenerateInst(UObject* _GameInstance);
	static USoundManager* Instance();
	static void DestroyManager();


	
};
