// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonCell.h"

#include "DungeonDataAsset.h"
#include "DungeonManager.h"
#include "PortFolio.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/PathManager.h"

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
			SetWall(true, 1);
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, 1)))
			SetWall(true, 2);
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(1, 0)))
			SetWall(true, 3);
		if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, -1)))
			SetWall(true, 4);
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
				SetWall(false, 1);
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, 1)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, 1)))
				SetWall(false, 2);
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(1, 0)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(1, 0)))
				SetWall(false, 3);
			if(UDungeonManager::Instance()->IsNotRoom(Matrix + FVector2D(0, -1)) && !UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, -1)))
				SetWall(false, 4);
		}
		// 방과 겹쳐있지 않을 때
		else
		{
			// 길이 아닌 방향에 벽 생성하기
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(-1, 0)))
				SetWall(true, 1);
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, 1)))
				SetWall(true, 2);
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(1, 0)))
				SetWall(true, 3);;
			if(UDungeonManager::Instance()->IsNotRoad(Matrix + FVector2D(0, -1)))
				SetWall(true, 4);
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

	// PathNode Obstacle 설정
	if(!bFloor)
	{
		for(auto& _Node : PathNodes)
			_Node.Value->bObstacle = true;
	}
	else
	{
		// 기본-> 전부 이동가능
		for(auto& _PathNode : PathNodes)
		{
			_PathNode.Value->bObstacle = false;
		}
		
		// Top, Right, Left, Bottom
		if(bTopWall || bTopWallSide)
			for(int i = 0; i <= 4; i++)
				PathNodes.FindRef(FVector2D(i, 4))->bObstacle = true;
		if(bRightWall || bRightWallSide)
			for(int i = 0; i <= 4; i++)
				PathNodes.FindRef(FVector2D(4, i))->bObstacle = true;
		if(bBottomWall || bBottoWallSide)
			for(int i = 0; i <= 4; i++)
				PathNodes.FindRef(FVector2D(i, 0))->bObstacle = true;
		if(bLeftWall || bLeftWallSide)
			for(int i = 0; i <= 4; i++)
				PathNodes.FindRef(FVector2D(0, i))->bObstacle = true;
	}
	
	// for(int i = 0; i <= 4; i++)
	// {
	// 	PathNodes.FindRef(FVector2D(i, 4))->bObstacle = bTopWall || bTopWallSide;
	// 	PathNodes.FindRef(FVector2D(4, i))->bObstacle = bRightWall || bRightWallSide;
	// 	PathNodes.FindRef(FVector2D(i, 0))->bObstacle = bBottomWall || bBottoWallSide;
	// 	PathNodes.FindRef(FVector2D(0, i))->bObstacle = bLeftWall || bLeftWallSide;
	// }
}

void UDungeonCell::SetWall(bool _Value, int _Dir)
{
	// 1-Left, 2-Top, 3-Right, 4-Bottom

	if(_Dir == 1)
	{
		bLeftWall = _Value;
		UDungeonCell* _Cell = UDungeonManager::Instance()->CellMap.FindRef(Matrix + FVector2D(-1, 0));
		if(_Cell != nullptr)
			_Cell->bRightWallSide = _Value;
	}
	else if(_Dir == 2)
	{
		bTopWall = _Value;
		UDungeonCell* _Cell = UDungeonManager::Instance()->CellMap.FindRef(Matrix + FVector2D(0, 1));
		if(_Cell != nullptr)
			_Cell->bBottoWallSide = _Value;
	}
	else if(_Dir == 3)
	{
		bRightWall = _Value;
		UDungeonCell* _Cell = UDungeonManager::Instance()->CellMap.FindRef(Matrix + FVector2D(1, 0));
		if(_Cell != nullptr)
			_Cell->bLeftWallSide = _Value;
	}
	else if(_Dir == 4)
	{
		bBottomWall = _Value;
		UDungeonCell* _Cell = UDungeonManager::Instance()->CellMap.FindRef(Matrix + FVector2D(0, -1));
		if(_Cell != nullptr)
			_Cell->bTopWallSide = _Value;
	}
}



