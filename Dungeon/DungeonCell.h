// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonManager.h"
#include "UObject/NoExportTypes.h"
#include "DungeonCell.generated.h"

/**
 * 
 */

class UDungeonRoom;
struct FPathNode;

UCLASS()
class PORTFOLIO_API UDungeonCell : public UObject
{
	GENERATED_BODY()

	// 좌표
public:
	FMatrix2D Matrix;
	FVector Location;
	UDungeonRoom* Room;

	// 던전 변수
public:
	bool bRoom, bRoad, bFloor;
	bool bLeftWall, bTopWall, bRightWall, bBottomWall;
	bool bLeftWallSide, bTopWallSide, bRightWallSide, bBottoWallSide;

	// 던전 레벨 액터들
public:
	AActor* Floor;
	AActor* LeftWall;
	AActor* TopWall;
	AActor* RightWall;
	AActor* BottomWall;
	TArray<AActor*> LevelActors;
	// 추가 오브젝트
	TMap<FString, AActor*> PropActors;

	// 길찾기 관련
public:
	TMap<FVector2D, FPathNode*> PathNodes;

	// 디버그 관련
public:
	AActor* Visualizer;

	// 초기화
public:
	void InitCell(int _X, int _Y);
	
	void ClearVisualizer();
	
	void GenerateLevel();
	void SpawnLevelActors();

	void SetWall(bool _Value, int _Dir);

	
	// Prop 관련
	void AddProp(TSubclassOf<AActor> _PropClass, FString _Name, bool _bOnWall = false);
	
	
};
