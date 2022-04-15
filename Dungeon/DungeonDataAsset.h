// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonDataAsset.generated.h"

class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UDungeonDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TesterFloor;

	// 레벨
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Floor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> LeftWall;

	// 몬스터
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AMonsterGeneralCharacter>> RoomMonsters;
	
	
};
