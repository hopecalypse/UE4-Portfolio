// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PortFolioGameModeBase.generated.h"

class APlayableCharacter;
class UPlayerHUD;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API APortFolioGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APortFolioGameModeBase();

protected:
	virtual void BeginPlay() override;


	// ! 필요 변수: UI
private:
	TSubclassOf<UUserWidget> PlayerHUDClass;
	UPlayerHUD* PlayerHUDWidget;
public:
	UPlayerHUD* GetPlayerHUDWidget() { return PlayerHUDWidget; };


	// 게임 시작 관련
private:
	TSubclassOf<APlayableCharacter> WarriorBP;
	TSubclassOf<APlayableCharacter> MageBP;
	TSubclassOf<APlayableCharacter> GunnerBP;
	
};
