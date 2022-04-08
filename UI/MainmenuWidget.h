// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainmenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMainmenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UButton* StartButton;
	UButton* ExitButton;

public:
	UFUNCTION(BlueprintCallable)
	void ClickStartButton();
	UFUNCTION(BlueprintCallable)
	void ClickExitButton();
	
	
	
};
