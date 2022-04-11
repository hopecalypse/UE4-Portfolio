// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PathManager.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPathManager : public UObject
{
	GENERATED_BODY()

private:
	static UPathManager* ManagerInstance;
public:
	static void GenerateInst(UObject* _GameInstance);
	static UPathManager* Instance();
	static void DestroyManager();

	// 길찾기 관련
	
	
};
