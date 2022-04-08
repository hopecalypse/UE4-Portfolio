// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/CustomEnums.h"
#include "SelectCharacterWidget.generated.h"

class UButton;
class ACharacterSelectGameMode;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API USelectCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 필요 변수
	ACharacterSelectGameMode* SelectGameMode;

	// 캐릭터 정보 팝업
private:
	UWidget* WarriorPopup;
	UWidget* MagePopup;
	UWidget* GunnerPopup;
	UButton* ConfirmButton;
	UButton* ExitButton;

	// 캐릭터 선택 위젯 띄우기
public:
	void OpenCharacterPopup(EPlayerClass _Class);

	// 캐릭터 선택 버튼
public:
	UFUNCTION(BlueprintCallable)
	void ConfirmSelectButton();

	UFUNCTION(BlueprintCallable)
	void ExitSelectButton();
	
	
};
