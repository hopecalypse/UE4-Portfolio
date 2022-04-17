// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PathManager.generated.h"

/**
 * 
 */

class UDungeonCell;
USTRUCT(Atomic)
struct FPathNode
{
	GENERATED_USTRUCT_BODY()
public:
	UDungeonCell* Cell;
	FVector2D Matrix;
	FVector Location;
	
	bool bObstacle;
	int F;		// 최종 평가 비용
	int G;		// 이동 비용
	int H;		// 예상 이동 비용(거리)
	FPathNode* Parent;

	FPathNode()
	{
		
	}
	FPathNode(int _X, int _Y)
	{
		Matrix = FVector2D(_X, _Y);
		Location = FVector(_Y * 150.f - 300.f, _X * 150.f - 300.f, 0.f);
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

	// 노드 Grid 리스트(1 Cell = 6x6 PathNode)
public:
	TMap<FVector2D, FPathNode*> PathNodeMap;
	TArray<FPathNode*> PathNodeArray;
	
	// 길찾기
public:
	TArray<FVector> FindPath(FVector _Start, FVector _End);
	FPathNode* FindCloseNode(FVector _Location);

	FPathNode* PopOpenList(TArray<FPathNode*>* _OpenList);
	
};
