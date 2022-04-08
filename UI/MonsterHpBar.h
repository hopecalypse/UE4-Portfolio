// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHpBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMonsterHpBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UProgressBar* HpProgressBar;
	void GetProgressBarObject();

public:
	void UpdateHpBar(float _CurrentHp, float _MaxHp);
	
};
