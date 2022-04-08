// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterSelectController.generated.h"

class ASelectCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API ACharacterSelectController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterSelectController();
protected:
	virtual void BeginPlay() override;
public:
	virtual void SetupInputComponent() override;

	// 캐릭터 선택 관련
private:
	ASelectCharacter* CurrentSelectCharacter;
	ASelectCharacter* PrevSelectCharacter;

public:
	APawn* DefaultPawn;

public:
	void LeftButtonPressed();
	void LeftButtonReleased();
	
};
