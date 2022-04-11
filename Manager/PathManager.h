// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PathManager.generated.h"

/**
 * 
 */

USTRUCT(Atomic)
struct FPathNode
{
	GENERATED_USTRUCT_BODY()
public:
	FVector2D Matrix;
	FVector Location;
	bool bLeftWall;
	bool bTopWall;
	bool bRightWall;
	bool bBottomWall;

	FPathNode()
	{
		
	}
	FPathNode(int _X, int _Y)
	{
		Matrix = FVector2D(_X, _Y);
		Location = FVector(100.f + _Y * 100.f, 100.f + _X * 100.f, 0.f);
	}
	
};


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

	// 노드 Grid 리스트
public:
	TMap<FVector2D, FPathNode*> PathNodeMap;
	
	// 길찾기 관련
public:
	
	
};
