// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MonsterHpBar.h"

#include "PortFolio.h"
#include "Components/ProgressBar.h"

void UMonsterHpBar::NativeConstruct()
{
	Super::NativeConstruct();

	GetProgressBarObject();
}

void UMonsterHpBar::UpdateHpBar(float _CurrentHp, float _MaxHp)
{
	if(HpProgressBar == nullptr)	GetProgressBarObject();
	float _Value =  _CurrentHp / _MaxHp;
	
	HpProgressBar->SetPercent(_Value);
}

void UMonsterHpBar::GetProgressBarObject()
{
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HpBar")));
}
