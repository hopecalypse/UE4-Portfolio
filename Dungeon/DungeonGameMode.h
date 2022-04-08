// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonGameMode.generated.h"

class UDungeonGeneratorWidget;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API ADungeonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADungeonGameMode();
private:
	virtual void BeginPlay() override;

	// 던전 Generator 위젯
private:
	TSubclassOf<UDungeonGeneratorWidget> GeneratorWidgetClass;
	UDungeonGeneratorWidget* DungeonGeneratorWidget;
	
	
};
