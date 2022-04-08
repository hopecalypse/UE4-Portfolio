// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectCharacter/CharacterSelectController.h"

#include "PortFolio.h"
#include "SelectCharacter.h"

ACharacterSelectController::ACharacterSelectController()
{
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ACharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 UI Only로 설정
	// FInputModeUIOnly _UIMode;
	// SetInputMode(_UIMode);
	// bShowMouseCursor = true;

	// 입력 모드 UI and Game으로
	FInputModeGameAndUI _InputMode;
	SetInputMode(_InputMode);
	bShowMouseCursor = true;

	LOGTEXT_WARN(TEXT("캐릭터 선택 컨트롤러 사용중"));

	DefaultPawn = GetPawn();
}

void ACharacterSelectController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
}

void ACharacterSelectController::LeftButtonPressed()
{
	
}

void ACharacterSelectController::LeftButtonReleased()
{
}
