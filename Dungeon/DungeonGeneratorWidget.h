// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonGeneratorWidget.generated.h"

class UTextBlock;
class UEditableText;
class UCanvasPanel;
class UButton;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UDungeonGeneratorWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

	// 기본
private:
	UButton* ProgressButton;
	UTextBlock* ButtonText;
	int Phase;
	int Visualize = 0;

	// 0. 그리드 생성
private:
	UCanvasPanel* GridCanvas;
	UEditableText* WidthSizeText;
	UEditableText* HeightSizeText;

	// 1. 나누기
	UCanvasPanel* SplitCanvas;
	UEditableText* SplitCountText;
	
	
private:
	UFUNCTION()
	void OnClickNextButton();
	
};
