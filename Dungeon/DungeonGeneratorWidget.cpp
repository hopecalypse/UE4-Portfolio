// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonGeneratorWidget.h"

#include "DungeonManager.h"
#include "PortFolio.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/PathManager.h"
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
	DescribeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("DescribeText")));

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
		DescribeText->SetText(FText::FromString(TEXT("BSP 트리 공간 분할")));
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
		DescribeText->SetText(FText::FromString(TEXT("랜덤 방 생성")));
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
		DescribeText->SetText(FText::FromString(TEXT("통로 생성")));
		Phase++;
	}
	// 6. 방 구현
	else if(Phase == 6)
	{
		UDungeonManager::Instance()->UpdateCells();
		UDungeonManager::Instance()->GenerateProps();
		DescribeText->SetText(FText::FromString(TEXT("레벨 액터 구현")));
		Phase++;
	}
	// 7. 방 분류하기
	else if(Phase == 7)
	{
		UDungeonManager::Instance()->SortRooms();
		DescribeText->SetText(FText::FromString(TEXT("플레이어 시작 / 보스 방 선택")));
		Phase++;
	}
	// 8. 길찾기 테스트: 이동가능 Node 표시
	else if(Phase == 8)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(UDungeonManager::Instance());
		// 모든 PathNode들에 대해 Obstacle 표시
		for(auto& _PathNode : UPathManager::Instance()->PathNodeMap)
		{
			FLinearColor _Color = _PathNode.Value->bObstacle ? FLinearColor::Red : FLinearColor::Green;
			UKismetSystemLibrary::DrawDebugPoint(this, _PathNode.Value->Location, 10.f, _Color, 100.f);
		}
		DescribeText->SetText(FText::FromString(TEXT("A* 패스파인딩 노드 생성")));
		Phase++;
	}
	// 9. 길찾기 테스트: 플레이어~보스 A* 경로탐색
	else if(Phase == 9)
	{
		UDungeonManager::Instance()->TestPathFinding();
		DescribeText->SetText(FText::FromString(TEXT("스테이지 진행 경로 탐색")));
		Phase++;
	}
	// 10. 몬스터 생성
	else if(Phase == 10)
	{
		UDungeonManager::Instance()->GenerateMonster();
		DescribeText->SetText(FText::FromString(TEXT("몬스터 생성")));
		ButtonText->SetText(FText::FromString(TEXT("시작")));
		Phase++;
	}
	// 999. 플레이어 스폰
	else if(Phase == 11)
	{
		UDungeonManager::Instance()->StartGame();
		SetVisibility(ESlateVisibility::Hidden);
	}
	
}
