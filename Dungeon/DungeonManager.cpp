// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonManager.h"

#include "DungeonCell.h"
#include "DungeonDataAsset.h"
#include "PortFolio.h"
#include "Kismet/KismetSystemLibrary.h"

UDungeonManager* UDungeonManager::ManagerInstance = nullptr;

UDungeonManager::UDungeonManager()
{
	static ConstructorHelpers::FObjectFinder<UDungeonDataAsset> _DungeonData(TEXT("DungeonDataAsset'/Game/_Blueprints/Dungeon/DungeonLevelDataAsset.DungeonLevelDataAsset'"));
	if(_DungeonData.Succeeded())
		LevelDataAsset = _DungeonData.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> _RedMaterial(TEXT("Material'/Game/_Materials/MT_Red.MT_Red'"));
	if(_RedMaterial.Succeeded())
		RedMaterial = _RedMaterial.Object;
}

void UDungeonManager::GenerateInst(UObject* _GameInstance)
{
	if(ManagerInstance == nullptr)
	{
		ManagerInstance = NewObject<UDungeonManager>(_GameInstance);
		ManagerInstance->AddToRoot();
	}
}

UDungeonManager* UDungeonManager::Instance()
{
	return ManagerInstance;
}

void UDungeonManager::DestroyManager()
{
	if(ManagerInstance != nullptr)
	{
		ManagerInstance->ConditionalBeginDestroy();
		ManagerInstance = nullptr;
	}
}

TArray<UDungeonCell*> UDungeonManager::GetCellsInside(FRect _Rect)
{
	TArray<UDungeonCell*> _CellArray;
	// _Rect 안에 있는 Cell들 구하기
	// 아래~위
	for(int y = _Rect.Y; y < _Rect.Y + _Rect.Height; y++)
	{
		for(int x = _Rect.X; x < _Rect.X + _Rect.Width; x++)
		{
			_CellArray.Add(CellList.FindRef(FVector2D(y, x)));
		}
	}
	return _CellArray;
}

void UDungeonManager::GenerateGrid(int _Width, int _Height)
{
	// Tester 액터 생성
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for(int i = 0; i < _Height; i++)
	{
		for(int j = 0; j < _Width; j++)
		{
			UDungeonCell* _GenCell = NewObject<UDungeonCell>(this);
			_GenCell->InitCell(j, i);
			CellList.Add(FVector2D(j, i), _GenCell);
			
			AActor* _Visualizer = GetWorld()->SpawnActor<AActor>(LevelDataAsset->TesterFloor, _GenCell->Location, FRotator::ZeroRotator, _SpawnParams);
			_GenCell->Visualizer = _Visualizer;
		}
	}

	// Root 노드 생성
	RootNode = new FTreeNode(0, 0, _Width, _Height);
	RootNode->bIsLeaf = false;
}

void UDungeonManager::StartSplitTree(int _Count)
{
	// Depth Array 만들기
	for(int i = 0; i <= _Count; i++)
	{
		TArray<FTreeNode*> _Arr;
		TreeDepthLists.Add(_Arr);
	}
	// Count만큼 재귀 분할 시작
	SplitTree(RootNode, _Count);
}

void UDungeonManager::SplitTree(FTreeNode* _TreeNode, int _Depth)
{
	// 끝 감지 조건: Count만큼 || 최소 사이즈(?)
	if(_Depth == 0 || _TreeNode->Rect.Width <= 3 || _TreeNode->Rect.Height <= 3)
	{
		_TreeNode->bIsLeaf = true;
		TreeDepthLists[TreeDepthLists.Num() - _Depth - 1].Add(_TreeNode);
		return;
	}
	
	// 트리 표시하기
	_TreeNode->bIsLeaf = false;
	TreeDepthLists[TreeDepthLists.Num() - _Depth - 1].Add(_TreeNode);
	LOGTEXT_LOG(TEXT("Dep[%d]:X(%d), Y(%d), Width(%d), Height(%d)"), _Depth, _TreeNode->Rect.X, _TreeNode->Rect.Y, _TreeNode->Rect.Width, _TreeNode->Rect.Height);

	// 랜덤 분할 비율 결정
	float _RandRatio = FMath::RandRange(0.4f, 0.6f);

	// 가로 분할
	if(_TreeNode->Rect.Width >= _TreeNode->Rect.Height)
	{
		// 분할 기준점 정하기
		int _Split = FMath::RoundToInt(_TreeNode->Rect.Width * _RandRatio);
		
		_TreeNode->Left = new FTreeNode(_TreeNode->Rect.X, _TreeNode->Rect.Y, _Split, _TreeNode->Rect.Height);
		_TreeNode->Right = new FTreeNode(_TreeNode->Rect.X + _Split, _TreeNode->Rect.Y, _TreeNode->Rect.Width - _Split, _TreeNode->Rect.Height);
	}
	// 세로 분할
	else
	{
		// 분할 기준점 정하기
		int _Split = FMath::RoundToInt(_TreeNode->Rect.Height * _RandRatio);
		
		_TreeNode->Left = new FTreeNode(_TreeNode->Rect.X, _TreeNode->Rect.Y, _TreeNode->Rect.Width, _Split);
		_TreeNode->Right = new FTreeNode(_TreeNode->Rect.X, _TreeNode->Rect.Y + _Split, _TreeNode->Rect.Width, _TreeNode->Rect.Height - _Split);
	}

	_TreeNode->Left->Parent = _TreeNode;
	_TreeNode->Right->Parent = _TreeNode;
	_Depth--;
	SplitTree(_TreeNode->Left, _Depth);
	SplitTree(_TreeNode->Right, _Depth);
}

bool UDungeonManager::VisualizeTree(int _Depth)
{
	//UKismetSystemLibrary::FlushPersistentDebugLines(this);

	bool _Flag = true;
	if(_Depth >= TreeDepthLists.Num() - 1)
		_Flag = false;
	//LOGTEXT_LOG(TEXT("Depth(%d) Visualize(Num:%d)"), _Depth, TreeDepthLists[_Depth].Num());
	
	for (auto _TreeNode : TreeDepthLists[_Depth])
	{
		FLinearColor _Color = FLinearColor::MakeRandomColor();
		
		// Bottom
		FVector _BottomStart = FVector(-280.f + _TreeNode->Rect.X * 600.f, _TreeNode->Rect.Y * 600.f - 280.f, 10.f * _Depth);
		FVector _BottomEnd = FVector(-280.f + (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f, _TreeNode->Rect.Y * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this,  _BottomStart, _BottomEnd, _Color, 10.f, 100.f);
		
		// Top
		FVector _TopStart = FVector(-280.f + _TreeNode->Rect.X * 600.f, (_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, 10.f * _Depth);
		FVector _TopEnd = FVector(-280.f + (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f, (_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _TopStart, _TopEnd, _Color, 10.f, 100.f);
		
		// Left
		FVector _LeftStart = FVector(_TreeNode->Rect.X * 600.f - 280.f, _TreeNode->Rect.Y * 600.f - 280.f, 10.f * _Depth);
		FVector _LeftEnd = FVector(_TreeNode->Rect.X * 600.f - 280.f, (_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _LeftStart, _LeftEnd, _Color, 10.f, 100.f);
		
		// Right
		FVector _RightStart = FVector((_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, _TreeNode->Rect.Y * 600.f - 280.f, 10.f * _Depth);
		FVector _RightEnd = FVector((_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, (_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _RightStart, _RightEnd, _Color, 10.f, 100.f);
	}
	
	return _Flag;
}

void UDungeonManager::StartGenerateRoom()
{
	GenerateRoom(RootNode);
}

void UDungeonManager::GenerateRoom(FTreeNode* _TreeNode)
{
	if(_TreeNode->bIsLeaf)
	{
		LOGTEXT_LOG(TEXT("방 생성 Rect: %d-%d, %d-%d"), _TreeNode->Rect.X, _TreeNode->Rect.Width, _TreeNode->Rect.Y, _TreeNode->Rect.Height);
	
		// 랜덤 방 크기 정하기(최소 2 ~ 최대 (길이-1))
		int _Width = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Width * 2 / 3), _TreeNode->Rect.Width - 1);
		//int _Width = FMath::RandRange(_TreeNode->Rect.Width / 2, _TreeNode->Rect.Width - 1);
		_Width = FMath::Clamp(_Width, 2, _TreeNode->Rect.Width - 1);
		int _Height = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Height *2 / 3), _TreeNode->Rect.Height - 1);
		//int _Height = FMath::RandRange(_TreeNode->Rect.Height / 2, _TreeNode->Rect.Height - 1);
		_Height = FMath::Clamp(_Height, 2, _TreeNode->Rect.Width - 1);

		// 랜덤 위치
		int _X = _TreeNode->Rect.X + FMath::RandRange(0, _TreeNode->Rect.Width - _Width);
		int _Y = _TreeNode->Rect.Y + FMath::RandRange(0, _TreeNode->Rect.Height - _Height);

		LOGTEXT_WARN(TEXT("방 결과 Rect: %d-%d, %d-%d"), _X, _Width, _Y, _Height);
		// 해당되는 Cell들 방으로 변환
		TArray<UDungeonCell*> _RoomCells = GetCellsInside(FRect(_X, _Y, _Width, _Height));
		for(int i = 0; i < _RoomCells.Num(); i++)
		{
			_RoomCells[i]->ClearVisualizer();
		}
	}

	else
	{
		GenerateRoom(_TreeNode->Left);
		GenerateRoom(_TreeNode->Right);
	}
}
