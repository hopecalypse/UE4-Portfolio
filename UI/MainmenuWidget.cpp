// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainmenuWidget.h"

#include "PortFolio.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UMainmenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 위젯 찾기
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
	
	// 클릭 델리게이트 바인드
	StartButton->OnClicked.AddDynamic(this, &UMainmenuWidget::ClickStartButton);
	ExitButton->OnClicked.AddDynamic(this, &UMainmenuWidget::ClickExitButton);
	
}

void UMainmenuWidget::ClickStartButton()
{
	LOGTEXT_LOG(TEXT("Start Game Button Clicked"));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("CharacterSelectMap"));
}

void UMainmenuWidget::ClickExitButton()
{
	LOGTEXT_LOG(TEXT("Exit Game Button Clicked"));
}
