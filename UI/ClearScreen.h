// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClearScreen.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UClearScreen : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void ClickMainMenuButton();

private:
	UButton* MainMenuButton;
	
};
