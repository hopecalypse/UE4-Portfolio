// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainmenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AMainmenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainmenuGameMode();

protected:
	virtual void BeginPlay() override;


	// 필요 변수: UI 위젯
	TSubclassOf<UUserWidget> MainmenuWidgetClass;
	UUserWidget* MainmenuWidget;
	
};
