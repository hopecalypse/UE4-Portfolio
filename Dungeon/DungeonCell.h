// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonCell.generated.h"

/**
 * 
 */

struct FPathNode;
UCLASS()
class PORTFOLIO_API UDungeonCell : public UObject
{
	GENERATED_BODY()

	// 좌표
public:
	FVector2D Matrix;
	FVector Location;
	AActor* Visualizer;

	// 던전 변수(할당용)
public:
	bool bRoom;
	bool bRoad;
	bool bFloor;
	bool bLeftWall;
	bool bTopWall;
	bool bRightWall;
	bool bBottomWall;

	// 던전 레벨 오브젝트들
public:
	AActor* Floor;
	AActor* LeftWall;
	AActor* TopWall;
	AActor* RightWall;
	AActor* BottomWall;
	TArray<AActor*> LevelActors;

	// 길찾기 관련
public:
	TArray<FPathNode*> PathNodes;

	// 초기화
public:
	void InitCell(int _X, int _Y);
	
	void ClearVisualizer();
	
	void GenerateLevel();
	void SpawnLevelActors();
	
};
