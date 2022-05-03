// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ClearScreen.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UClearScreen::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton = Cast<UButton>(GetWidgetFromName(TEXT("MainMenuButton")));

	MainMenuButton->OnClicked.AddDynamic(this, &UClearScreen::ClickMainMenuButton);
}

void UClearScreen::ClickMainMenuButton()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("StartMap"));
}
