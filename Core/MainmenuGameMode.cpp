// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainmenuGameMode.h"

#include "PortFolio.h"
#include "Blueprint/UserWidget.h"

AMainmenuGameMode::AMainmenuGameMode()
{
	// 위젯 BP 찾기
	static ConstructorHelpers::FClassFinder<UUserWidget> _MainmenuWidgetBP(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_MainmenuWidget.WBP_MainmenuWidget_C'"));
	if(_MainmenuWidgetBP.Succeeded())
		MainmenuWidgetClass = _MainmenuWidgetBP.Class; 
}

void AMainmenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 위젯 생성하기
	if(MainmenuWidgetClass != nullptr)
	{
		MainmenuWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), MainmenuWidgetClass));
		if(MainmenuWidget != nullptr)
			MainmenuWidget->AddToViewport();
	}
	
	FInputModeUIOnly _InputMode;
	GetWorld()->GetFirstPlayerController()->SetInputMode(_InputMode);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
}
