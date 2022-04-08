// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectCharacterWidget.h"

#include "PortFolio.h"
#include "Components/Button.h"
#include "Core/CharacterSelectGameMode.h"
#include "Kismet/GameplayStatics.h"

void USelectCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	WarriorPopup = GetWidgetFromName(TEXT("WarriorPopup"));
	MagePopup = GetWidgetFromName(TEXT("MagePopup"));
	GunnerPopup = GetWidgetFromName(TEXT("GunnerPopup"));

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("SelectButton")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
	
	ConfirmButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::ConfirmSelectButton);
	ExitButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::ExitSelectButton);
}

void USelectCharacterWidget::OpenCharacterPopup(EPlayerClass _Class)
{
	// 다른것 지우고 받은 클래스만 띄우기
	if(_Class == EPlayerClass::E_Warrior)
	{
		WarriorPopup->SetVisibility(ESlateVisibility::Visible);
		MagePopup->SetVisibility(ESlateVisibility::Hidden);
		GunnerPopup->SetVisibility(ESlateVisibility::Hidden);
	}

	else if(_Class == EPlayerClass::E_Mage)
	{
		WarriorPopup->SetVisibility(ESlateVisibility::Hidden);
		MagePopup->SetVisibility(ESlateVisibility::Visible);
		GunnerPopup->SetVisibility(ESlateVisibility::Hidden);
	}

	else if(_Class == EPlayerClass::E_Gunner)
	{
		WarriorPopup->SetVisibility(ESlateVisibility::Hidden);
		MagePopup->SetVisibility(ESlateVisibility::Hidden);
		GunnerPopup->SetVisibility(ESlateVisibility::Visible);
	}
	
	ConfirmButton->SetVisibility(ESlateVisibility::Visible);
	ExitButton->SetVisibility(ESlateVisibility::Visible);
}

void USelectCharacterWidget::ConfirmSelectButton()
{
	if(SelectGameMode == nullptr)
		SelectGameMode = Cast<ACharacterSelectGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	SelectGameMode->ConfirmSelect_FromButton();
}

void USelectCharacterWidget::ExitSelectButton()
{
	if(SelectGameMode == nullptr)
		SelectGameMode = Cast<ACharacterSelectGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// 선택 팝업 UI들 끄기
	WarriorPopup->SetVisibility(ESlateVisibility::Hidden);
	MagePopup->SetVisibility(ESlateVisibility::Hidden);
	GunnerPopup->SetVisibility(ESlateVisibility::Hidden);
	
	ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
	ExitButton->SetVisibility(ESlateVisibility::Hidden);

	SelectGameMode->ExitSelect_FromButton();
}
