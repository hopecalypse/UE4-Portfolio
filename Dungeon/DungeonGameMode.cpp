// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonGameMode.h"

#include "DungeonGeneratorWidget.h"

ADungeonGameMode::ADungeonGameMode()
{
	// Generator 위젯 BP Class
	static ConstructorHelpers::FClassFinder<UDungeonGeneratorWidget> _GeneratorClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_DungenGenerator.WBP_DungenGenerator_C'"));
	if(_GeneratorClass.Succeeded())
		GeneratorWidgetClass = _GeneratorClass.Class;
}

void ADungeonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Generator 위젯 생성
	DungeonGeneratorWidget = Cast<UDungeonGeneratorWidget>(CreateWidget(GetWorld(), GeneratorWidgetClass));
	DungeonGeneratorWidget->AddToViewport();

	// Input Mode 설정
	//FInputModeUIOnly _Mode;
	FInputModeGameAndUI _Mode;
	//FInputModeGameOnly _Mode;
	GetWorld()->GetFirstPlayerController()->SetInputMode(_Mode);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	
}
