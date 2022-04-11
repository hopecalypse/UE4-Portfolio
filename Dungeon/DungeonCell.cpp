// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonCell.h"

#include "DungeonDataAsset.h"
#include "DungeonManager.h"
#include "PortFolio.h"

void UDungeonCell::InitCell(int _X, int _Y)
{
	Matrix = FVector2D(_X, _Y);
	// 언리얼 좌표계상 정면 = X, 우측 = Y 
	Location = FVector(_Y * 600.f, _X * 600.f, 0.f);
}

void UDungeonCell::ClearVisualizer()
{
	if(Visualizer != nullptr)
		Visualizer->Destroy();
}

void UDungeonCell::GenerateLevel()
{
	// 1. Room
	if(bRoom)
	{
		bFloor = true;
		// 인접한 Cell도 Room이 아닐 시 벽세우기
		// Left, Top, Right, Bottom
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(-1, 0)))
			bLeftWall = true;
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, 1)))
			bTopWall = true;
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(1, 0)))
			bRightWall = true;
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, -1)))
			bBottomWall = true;
	}

	// 2. Road
	else if(bRoad)
	{
		
	}
	
	SpawnLevelActors();
}

void UDungeonCell::SpawnLevelActors()
{
	// Visualizer 삭제
	ClearVisualizer();

	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = false;
	
	if(bFloor && Floor == nullptr)
		GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->Floor, Location, FRotator::ZeroRotator, _SpawnParams);
	if(bLeftWall && LeftWall == nullptr)
		GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator::ZeroRotator, _SpawnParams);
	if(bTopWall && TopWall == nullptr)
		GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 90.f, 0.f), _SpawnParams);
	if(bRightWall && RightWall == nullptr)
		GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 180.f, 0.f), _SpawnParams);
	if(bBottomWall && BottomWall == nullptr)
		GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 270.f, 0.f), _SpawnParams);

	if(!bFloor && Floor != nullptr)
		Floor->Destroy();
	if(!bLeftWall && LeftWall != nullptr)
		LeftWall->Destroy();
	if(!bTopWall && TopWall != nullptr)
		TopWall->Destroy();
	if(!bBottomWall && BottomWall != nullptr)
		BottomWall->Destroy();
}



