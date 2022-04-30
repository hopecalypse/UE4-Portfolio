// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonDataAsset.generated.h"

class ABossMonsterBase;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> DirectionalLight;

	// 레벨
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Floor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> LeftWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> LeftDoorWall;

	// 몬스터
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AMonsterGeneralCharacter>> RoomMonsters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABossMonsterBase> BossMonster;

	// Prop들
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> RoomCeilingLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> RoomRectLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> LeftWallLamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Pillar;
	
};
