// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonGeneratorWidget.h"

#include "DungeonManager.h"
#include "PortFolio.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/DefaultValueHelper.h"

void UDungeonGeneratorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 오브젝트 찾기
	ProgressButton = Cast<UButton>(GetWidgetFromName(TEXT("ProgressButton")));
	WidthSizeText = Cast<UEditableText>(GetWidgetFromName(TEXT("WidthSizeText")));
	HeightSizeText = Cast<UEditableText>(GetWidgetFromName(TEXT("HeightSizeText")));
	GridCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("GridCanvas")));
	ButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ButtonText")));
	SplitCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("SplitCanvas")));
	SplitCountText = Cast<UEditableText>(GetWidgetFromName(TEXT("SplitCountText")));

	// 델리게이트 등록
	ProgressButton->OnClicked.AddDynamic(this, &UDungeonGeneratorWidget::OnClickNextButton);
}

void UDungeonGeneratorWidget::OnClickNextButton()
{
	// 생성
	if(Phase == 0)
	{
		int _Width; int _Height;
		FDefaultValueHelper::ParseInt(WidthSizeText->GetText().ToString(), _Width);
		FDefaultValueHelper::ParseInt(HeightSizeText->GetText().ToString(), _Height);
		UDungeonManager::Instance()->GenerateGrid(_Width, _Height);
		
		GridCanvas->SetVisibility(ESlateVisibility::Hidden);
		SplitCanvas->SetVisibility(ESlateVisibility::Visible);
		ButtonText->SetText(FText::FromString(TEXT("분할")));
		Phase++;
	}
	// 1. 분할
	else if(Phase == 1)
	{
		int _SplitCount;
		FDefaultValueHelper::ParseInt(SplitCountText->GetText().ToString(), _SplitCount);
		UDungeonManager::Instance()->StartSplitTree(_SplitCount);
		
		SplitCanvas->SetVisibility(ESlateVisibility::Hidden);
		ButtonText->SetText(FText::FromString(TEXT("진행")));
		Phase++;
	}
	// 2. 그리기
	else if(Phase == 2) 
	{
		bool _Flag = UDungeonManager::Instance()->VisualizeTree(Visualize);
		if(_Flag)
			Visualize++;
		else
			Phase++;
	}
	// 3. 방 정하기
	else if(Phase == 3)
	{
		UDungeonManager::Instance()->StartGenerateRoom();
		Phase++;
	}
	// 4. 방 구현
	else if(Phase == 4)
	{
		UDungeonManager::Instance()->UpdateCells();
		Phase++;
	}
	// 5. 길 연결
	else if(Phase == 5)
	{
		UDungeonManager::Instance()->StartGenerateRoad();
		Phase++;
	}
	// 6. 방 구현
	else if(Phase == 6)
	{
		UDungeonManager::Instance()->UpdateCells();
		Phase++;
	}
	// 7. 방 분류하기
	else if(Phase == 7)
	{
		UDungeonManager::Instance()->SortRooms();
		Phase++;
	}
	// 8. 몬스터 생성
	else if(Phase == 8)
	{
		UDungeonManager::Instance()->GenerateMonster();
		Phase++;
	}
	// 999. 플레이어 스폰
	else if(Phase == 9)
	{
		UDungeonManager::Instance()->StartGame();
		SetVisibility(ESlateVisibility::Hidden);
	}
	
}
