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

void UDungeonCell::GenerateRoom(ECellType _Type, FActorSpawnParameters _SpawnParam)
{
	
}
