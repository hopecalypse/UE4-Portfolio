// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonRoom.generated.h"

class UDungeonCell;
struct FRect;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UDungeonRoom : public UObject
{
	GENERATED_BODY()

public:
	TArray<UDungeonCell*> Cells;
	FRect* Rect;
	UDungeonCell* CenterCell;

public:
	bool bPlayerStart;
	bool bBossRoom;
	
};
