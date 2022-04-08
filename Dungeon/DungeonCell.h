// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonCell.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECellType : uint8
{
	Floor,
	LeftWall,
	TopWall,
	RightWall,
	BottomWall,
};


UCLASS()
class PORTFOLIO_API UDungeonCell : public UObject
{
	GENERATED_BODY()

	// 좌표
public:
	FVector2D Matrix;
	FVector Location;
	AActor* Visualizer;

	// 던전 레벨 오브젝트들
public:
	AActor* Floor;
	AActor* LeftWall;
	AActor* TopWall;
	AActor* RightWall;
	AActor* BottomWall;

	// 초기화
public:
	void InitCell(int _X, int _Y);

	// 방 생성
	void ClearVisualizer();
	void GenerateRoom(ECellType _Type, FActorSpawnParameters _SpawnParam = {});
	
};
