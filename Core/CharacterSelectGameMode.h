// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterSelectGameMode.generated.h"

class ACharacterSelectController;
class USelectCharacterWidget;
class ASelectCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API ACharacterSelectGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	ACharacterSelectGameMode();
protected:
	virtual void BeginPlay() override;

	// 선택 UI 관련
private:
	TSubclassOf<USelectCharacterWidget> SelectCharacterWidgetClass;
	USelectCharacterWidget* SelectCharacterWidget;

	// 캐릭터 선택 관련
private:
	ACharacterSelectController* SelectController;
	ASelectCharacter* CurrentSelectCharacter;
	ASelectCharacter* PrevSelectCharacter;

public:
	// 캐릭터 클릭 시
	void Select_FromCharacter(ASelectCharacter* _SelectCharacter);
	// 다시 선택창으로 돌아가기
	void ExitSelect_FromButton();
	// 선택 버튼 확정 시
	void ConfirmSelect_FromButton();
	
};
