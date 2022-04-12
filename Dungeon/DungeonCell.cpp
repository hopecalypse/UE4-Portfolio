// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonCell.h"

#include "DungeonDataAsset.h"
#include "DungeonManager.h"
#include "PortFolio.h"
#include "Kismet/KismetSystemLibrary.h"

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
	if(bRoad)
	{
		bFloor = true;
		
		// 방과 겹쳐있을 때
		if(bRoom)
		{
			// 방과 이어지는 부분 벽을 제거하기
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(-1, 0)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(-1, 0)))
				bLeftWall = false;
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, 1)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, 1)))
				bTopWall = false;
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(1, 0)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(1, 0)))
				bRightWall = false;
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, -1)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, -1)))
				bBottomWall = false;
		}
		// 방과 겹쳐있지 않을 때
		else
		{
			// 길이 아닌 방향에 벽 생성하기
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(-1, 0)))
				bLeftWall = true;
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, 1)))
				bTopWall = true;
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(1, 0)))
				bRightWall = true;
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, -1)))
				bBottomWall = true;
		}
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
		Floor = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->Floor, Location, FRotator::ZeroRotator, _SpawnParams);
	if(bLeftWall && LeftWall == nullptr)
		LeftWall = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator::ZeroRotator, _SpawnParams);
	if(bTopWall && TopWall == nullptr)
		TopWall = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 90.f, 0.f), _SpawnParams);
	if(bRightWall && RightWall == nullptr)
		RightWall = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 180.f, 0.f), _SpawnParams);
	if(bBottomWall && BottomWall == nullptr)
		BottomWall = GetOuter()->GetWorld()->SpawnActor<AActor>(UDungeonManager::Instance()->GetLevelData()->LeftWall, Location, FRotator(0.f, 270.f, 0.f), _SpawnParams);

	if(!bFloor && Floor != nullptr)
		Floor->Destroy();
	if(!bLeftWall && LeftWall != nullptr)
		LeftWall->Destroy();
	if(!bTopWall && TopWall != nullptr)
		TopWall->Destroy();
	if(!bRightWall && RightWall != nullptr)
		RightWall->Destroy();
	if(!bBottomWall && BottomWall != nullptr)
		BottomWall->Destroy();
}



