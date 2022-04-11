// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonDataAsset.generated.h"

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Floor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> LeftWall;
	
};
