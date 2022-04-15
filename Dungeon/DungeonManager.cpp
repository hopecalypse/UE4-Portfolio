// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonManager.h"

#include "DungeonCell.h"
#include "DungeonDataAsset.h"
#include "DungeonRoom.h"
#include "ObserverPawn.h"
#include "PortFolio.h"
#include "Core/GameManagerInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/PathManager.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Player/PlayableCharacter.h"
#include "UI/PlayerHUD.h"

UDungeonManager* UDungeonManager::ManagerInstance = nullptr;

UDungeonManager::UDungeonManager()
{
	static ConstructorHelpers::FObjectFinder<UDungeonDataAsset> _DungeonData(TEXT("DungeonDataAsset'/Game/_Blueprints/Dungeon/DungeonLevelDataAsset.DungeonLevelDataAsset'"));
	if(_DungeonData.Succeeded())
		LevelDataAsset = _DungeonData.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> _RedMaterial(TEXT("Material'/Game/_Materials/MT_Red.MT_Red'"));
	if(_RedMaterial.Succeeded())
		RedMaterial = _RedMaterial.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> _PlayerHUDClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_PlayerHUD.WBP_PlayerHUD_C'"));
	if(_PlayerHUDClass.Succeeded())
		PlayerHUDBP = _PlayerHUDClass.Class;
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
			_CellArray.Add(CellMap.FindRef(FVector2D(x, y)));
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
			CellMap.Add(FVector2D(j, i), _GenCell);
			
			AActor* _Visualizer = GetWorld()->SpawnActor<AActor>(LevelDataAsset->TesterFloor, _GenCell->Location, FRotator::ZeroRotator, _SpawnParams);
			_GenCell->Visualizer = _Visualizer;

			// 9개의 PathNode 추가
			for(int k = 0; k < 3; k++)
			{
				for(int l = 0; l < 3; l++)
				{
					FPathNode* _PathNode = new FPathNode((j * 3) + l, (i * 3) + k);
					_GenCell->PathNodes.Add(_PathNode);
					UPathManager::Instance()->PathNodeMap.Add(FVector2D(_PathNode->Matrix), _PathNode);
				}
			}
		}
	}

	// Root 노드 생성
	RootNode = new FTreeNode(0, 0, _Width, _Height);
	RootNode->bIsLeaf = false;

	// 옵저버 플레이어 위치 조정
	APawn* _ObserverPlayer = UGameplayStatics::GetPlayerPawn(GetOuter(), 0);
	float _X = _Width * 300.f;
	float _Y = _Height * 300.f;
	_ObserverPlayer->SetActorLocation(FVector(_X, _Y, FMath::Max(_Width, _Height) * 800.f));
	_ObserverPlayer->SetActorRotation(FRotator(-90.f, 0.f, 0.f));
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
		FVector _BottomStart = FVector(_TreeNode->Rect.Y * 600.f - 280.f, -280.f + _TreeNode->Rect.X * 600.f, 10.f * _Depth);
		FVector _BottomEnd = FVector(_TreeNode->Rect.Y * 600.f - 280.f, -280.f + (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this,  _BottomStart, _BottomEnd, _Color, 10.f, 100.f);
		
		// Top
		FVector _TopStart = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, -280.f + _TreeNode->Rect.X * 600.f, 10.f * _Depth);
		FVector _TopEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, -280.f + (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _TopStart, _TopEnd, _Color, 10.f, 100.f);
		
		// Left
		FVector _LeftStart = FVector(_TreeNode->Rect.Y * 600.f - 280.f, _TreeNode->Rect.X * 600.f - 280.f, 10.f * _Depth);
		FVector _LeftEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, _TreeNode->Rect.X * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _LeftStart, _LeftEnd, _Color, 10.f, 100.f);
		
		// Right
		FVector _RightStart = FVector(_TreeNode->Rect.Y * 600.f - 280.f, (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, 10.f * _Depth);
		FVector _RightEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, 10.f * _Depth);
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
		// 랜덤 방 크기 정하기(최소 2 ~ 최대 (길이-1))
		int _Width = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Width / 2) + 1, _TreeNode->Rect.Width * 2 / 3);
		_Width = FMath::Clamp(_Width, 2, _TreeNode->Rect.Width - 1);
		int _Height = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Height / 2) + 1, _TreeNode->Rect.Height * 2 / 3);
		_Height = FMath::Clamp(_Height, 2, _TreeNode->Rect.Width - 1);

		// 랜덤 위치
		int _X = _TreeNode->Rect.X + FMath::RandRange(0, _TreeNode->Rect.Width - _Width);
		int _Y = _TreeNode->Rect.Y + FMath::RandRange(0, _TreeNode->Rect.Height - _Height);

		// Room 오브젝트 생성하기
		UDungeonRoom* _Room = NewObject<UDungeonRoom>(this);
		
		// 해당되는 Cell들 방으로 변환
		TArray<UDungeonCell*> _RoomCells = GetCellsInside(FRect(_X, _Y, _Width, _Height));
		for(int i = 0; i < _RoomCells.Num(); i++)
		{
			_RoomCells[i]->ClearVisualizer();
			_RoomCells[i]->bRoom = true;
			
			_Room->Cells.Add(_RoomCells[i]);
			_RoomCells[i]->Room = _Room;
		}
		
		// Room의 Rect 할당
		FRect* _Rect = new FRect(_X, _Y, _Width, _Height);
		_Room->Rect = _Rect;
		_Room->CenterCell = CellMap.FindRef(FVector2D(_X + FMath::RoundToInt(_Width / 2), _Y + FMath::RoundToInt(_Height / 2)));

		RoomList.Add(_Room);
		//UKismetSystemLibrary::DrawDebugCircle(this, _Room->CenterCell->Location, 100.f, 10.f, FLinearColor::Green, 100.f, 50.f);
	}

	else
	{
		GenerateRoom(_TreeNode->Left);
		GenerateRoom(_TreeNode->Right);
	}
}

bool UDungeonManager::IsNotRoom(FVector2D _Matrix)
{
	UDungeonCell* _Cell = CellMap.FindRef(_Matrix);
	if(_Cell == nullptr)
		return true;
	else
	{
		if(_Cell->bRoom)
			return false;
		else
			return true;
	}
}

void UDungeonManager::StartGenerateRoad()
{
	GenerateRoad(RootNode);
}

void UDungeonManager::GenerateRoad(FTreeNode* _TreeNode)
{
	if(_TreeNode == nullptr)
		return;

	if(_TreeNode->bIsLeaf)
		return;
	
	// Rect의 중앙 ~ 중앙까지 연결
	FVector2D _LeftCenter = GetRectCenter(_TreeNode->Left->Rect);
	FVector2D _RightCenter = GetRectCenter(_TreeNode->Right->Rect);

	// 가로
	for(int i = FMath::Min(_LeftCenter.X, _RightCenter.X); i <= FMath::Max(_LeftCenter.X, _RightCenter.X); i++)
	{
		UDungeonCell* _RoadCell = CellMap.FindRef(FVector2D(i, _LeftCenter.Y));
		_RoadCell->bRoad = true;
		UKismetSystemLibrary::DrawDebugBox(this, _RoadCell->Location + FVector(0.f, 0.f, 100.f), FVector(280.f), FLinearColor::Blue, FRotator::ZeroRotator, 10.f);
	}

	// 세로
	for(int i = FMath::Min(_LeftCenter.Y, _RightCenter.Y); i <= FMath::Max(_LeftCenter.Y, _RightCenter.Y); i++)
	{
		UDungeonCell* _RoadCell = CellMap.FindRef(FVector2D(_LeftCenter.X, i));
		_RoadCell->bRoad = true;
		UKismetSystemLibrary::DrawDebugBox(this, _RoadCell->Location + FVector(0.f, 0.f, 100.f), FVector(280.f), FLinearColor::Red, FRotator::ZeroRotator, 10.f);
	}
	
	GenerateRoad(_TreeNode->Left);
	GenerateRoad(_TreeNode->Right);
}

FVector2D UDungeonManager::GetRectCenter(FRect _Rect)
{
	return FVector2D(_Rect.X + FMath::RoundToInt(_Rect.Width / 2), _Rect.Y + FMath::RoundToInt(_Rect.Height / 2));
}

bool UDungeonManager::IsNotRoad(FVector2D _Matrix)
{
	UDungeonCell* _Cell = CellMap.FindRef(_Matrix);
	if(_Cell == nullptr)
		return true;
	else
	{
		if(_Cell->bRoad)
			return false;
		else
			return true;
	}
}

void UDungeonManager::SortRooms()
{
	// Bottom Left(Min), Top Right(Max) 골라내기
	UDungeonRoom* _BottomLeft = RoomList[0];
	UDungeonRoom* _TopLeft = RoomList[0];
	for(int i = 1; i < RoomList.Num(); i++)
	{
		int _Cur = RoomList[i]->CenterCell->Matrix.X + RoomList[i]->CenterCell->Matrix.Y;
		
		// 최소
		if(_Cur < (_BottomLeft->CenterCell->Matrix.X +_BottomLeft->CenterCell->Matrix.Y))
			_BottomLeft = RoomList[i];
		// 최대
		if(_Cur > (_TopLeft->CenterCell->Matrix.X + _TopLeft->CenterCell->Matrix.Y))
			_TopLeft = RoomList[i];
	}

	// 두 개의 방 중 랜덤 선택
	int _Rand = FMath::RandRange(0, 1);
	PlayerStartRoom = _Rand == 0 ? _BottomLeft : _TopLeft;
	BossRoom = _Rand == 0 ? _TopLeft : _BottomLeft;
	PlayerStartRoom->bPlayerStart = true;
	BossRoom->bBossRoom = true;

	UKismetSystemLibrary::DrawDebugSphere(this, PlayerStartRoom->CenterCell->Location, 100.f, 10, FLinearColor::Blue, 10.f, 50.f);
	UKismetSystemLibrary::DrawDebugSphere(this, BossRoom->CenterCell->Location, 100.f, 10, FLinearColor::Red, 10.f, 50.f);
}

void UDungeonManager::GenerateMonster()
{
	// 플레이어 시작방, 보스방이 아닌 방에 몬스터 랜덤 생성하기
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(!RoomList[i]->bPlayerStart && !RoomList[i]->bBossRoom)
		{
			UDungeonRoom* _CurRoom = RoomList[i];
				
			// 몬스터 개수
			int _Count = FMath::RandRange(3, 6);

			TArray<FVector2D> _Selected;
			for(int j = 0; j < _Count; j++)
			{
				// 랜덤 위치 설정(Cell)
				FVector2D _RandMatrix = FVector2D(FMath::RandRange(_CurRoom->Rect->X, _CurRoom->Rect->X + _CurRoom->Rect->Width), FMath::RandRange(_CurRoom->Rect->Y, _CurRoom->Rect->Y + _CurRoom->Rect->Height));;

				while(_Selected.Contains(_RandMatrix) == true)
					_RandMatrix = FVector2D(FMath::RandRange(_CurRoom->Rect->X, _CurRoom->Rect->X + _CurRoom->Rect->Width), FMath::RandRange(_CurRoom->Rect->Y, _CurRoom->Rect->Y + _CurRoom->Rect->Height));
				_Selected.Add(_RandMatrix);

				// 랜덤 몬스터 스폰
				TSubclassOf<AMonsterGeneralCharacter> _MonsterClass = LevelDataAsset->RoomMonsters[FMath::RandRange(0, LevelDataAsset->RoomMonsters.Num() - 1)];
				FActorSpawnParameters _SpawnParams;
				_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
				_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AMonsterGeneralCharacter* _Monster = GetOuter()->GetWorld()->SpawnActor<AMonsterGeneralCharacter>(_MonsterClass, CellMap.FindRef(_RandMatrix)->Location + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator, _SpawnParams);
			}
		}
	}
}

void UDungeonManager::UpdateCells()
{
	for (auto _Cell : CellMap)
	{
		_Cell.Value->GenerateLevel();
	}
}

void UDungeonManager::StartGame()
{
	// 플레이어 HUD 생성
	PlayerHUD = Cast<UPlayerHUD>(CreateWidget(GetOuter()->GetWorld(), PlayerHUDBP));
	PlayerHUD->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->AddToViewport();
	
	// 플레이어 스폰하기
	ObserverPawn = UGameplayStatics::GetPlayerPawn(GetOuter(), 0);
	Cast<AObserverPawn>(ObserverPawn)->bFreeze = true;
	TSubclassOf<APlayableCharacter> _SelPlayerClass = Cast<UGameManagerInstance>(UGameplayStatics::GetGameInstance(GetOuter()))->GetSelectedClassBP();
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Player = GetOuter()->GetWorld()->SpawnActor<APlayableCharacter>(_SelPlayerClass, PlayerStartRoom->CenterCell->Location + FVector(0.f, 0.f, 88.f), FRotator::ZeroRotator, _SpawnParams);
	Player->SetPlayerHUD(PlayerHUD);
	
	// InputMode 변경
	APlayerController* _PlayerController = UGameplayStatics::GetPlayerController(GetOuter(), 0);
	FInputModeGameOnly _GameInputMode;
	_PlayerController->SetInputMode(_GameInputMode);
	_PlayerController->SetShowMouseCursor(false);
	// 플레이어 Pawn으로 Blend 시작
	_PlayerController->SetViewTargetWithBlend(Player, 3.f);
	
	// 타이머 설정
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, TEXT("OnEndBlendToPlayer"));

	GetOuter()->GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, 3.f, false);
}

void UDungeonManager::OnEndBlendToPlayer()
{
	// ObserverPawn 제거
	ObserverPawn->Destroy();
	// 빙의하기
	APlayerController* _PlayerController = UGameplayStatics::GetPlayerController(GetOuter(), 0);
	_PlayerController->Possess(Player);
	// Debug 지우기
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}



