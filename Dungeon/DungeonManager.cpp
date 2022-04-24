// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonManager.h"

#include "DungeonCell.h"
#include "DungeonDataAsset.h"
#include "DungeonRoom.h"
#include "ObserverPawn.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "PortFolio.h"
#include "Core/GameManagerInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	// 정보 저장
	XSize = _Width;
	YSize = _Height;
	
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
			_GenCell->bRoom = false;
			_GenCell->bRoad = false;
			
			// 16개의 PathNode 추가
			for(int k = 0; k <= 4; k++)
			{
				for(int l = 0; l <= 4; l++)
				{
					FPathNode* _PathNode;
					if(!UPathManager::Instance()->PathNodeMap.Contains(FVector2D((j * 4) + l, (i * 4) + k)))
					{
						_PathNode = new FPathNode((j * 4) + l, (i * 4) + k);
						UPathManager::Instance()->PathNodeMap.Add(_PathNode->Matrix, _PathNode);
						UPathManager::Instance()->PathNodeArray.AddUnique(_PathNode);
					}
					else
					{
						_PathNode = UPathManager::Instance()->PathNodeMap.FindRef(FVector2D((j * 4) + l, (i * 4) + k));
					}
					
					// 해당 Cell 내부 의 Node로 할당
					FVector2D _CellMatrix = FVector2D(l, k);
					_GenCell->PathNodes.Add(_CellMatrix, _PathNode);
					_PathNode->Cell = _GenCell;
					//UKismetSystemLibrary::DrawDebugPoint(this, _PathNode->Location, 10.f + k + l, _Color, 100.f);
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
	_ObserverPlayer->SetActorLocation(FVector(_X, _Y, FMath::Max(_Width, _Height) * 700.f));
	_ObserverPlayer->SetActorRotation(FRotator(-90.f, 0.f, 0.f));

	// 글로벌 라이트 생성
	DirectionalLight = GetWorld()->SpawnActor<AActor>(LevelDataAsset->DirectionalLight, FVector::ZeroVector, FRotator::ZeroRotator, _SpawnParams);
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
			_RoomCells[i]->Room = _Room;
			
			_Room->Cells.Add(_RoomCells[i]);
		}
		
		// Room의 Rect 할당
		FRect* _Rect = new FRect(_X, _Y, _Width, _Height);
		_Room->Rect = _Rect;
		_Room->CenterCell = CellMap.FindRef(FVector2D(_X + FMath::RoundToInt(_Width / 2), _Y + FMath::RoundToInt(_Height / 2)));

		RoomList.Add(_Room);
		LOGTEXT_LOG(TEXT("Room 생성됨: (Cell개수:%d)"), _Room->Cells.Num());
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

		// Road 리스트에 추가
		RoadList.AddUnique(_RoadCell);
	}

	// 세로
	for(int i = FMath::Min(_LeftCenter.Y, _RightCenter.Y); i <= FMath::Max(_LeftCenter.Y, _RightCenter.Y); i++)
	{
		UDungeonCell* _RoadCell = CellMap.FindRef(FVector2D(_LeftCenter.X, i));
		_RoadCell->bRoad = true;
		UKismetSystemLibrary::DrawDebugBox(this, _RoadCell->Location + FVector(0.f, 0.f, 100.f), FVector(280.f), FLinearColor::Red, FRotator::ZeroRotator, 10.f);

		// Road 리스트에 추가
		RoadList.AddUnique(_RoadCell);
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

void UDungeonManager::TestPathFinding()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	
	FVector _Player = PlayerStartRoom->CenterCell->Location;
	FVector _End = BossRoom->CenterCell->Location;
	TArray<FVector> _PathArray = UPathManager::Instance()->FindPath(PlayerStartRoom->CenterCell->Location, BossRoom->CenterCell->Location);
	FVector _Current = _PathArray.Pop();
	FVector _Next;
	while (_PathArray.Num() > 0)
	{
		_Next = _PathArray.Pop();
		UKismetSystemLibrary::DrawDebugLine(this, _Current, _Next, FLinearColor::Blue, 100.f, 100.f);
		_Current = _Next;
	}
	UKismetSystemLibrary::DrawDebugPoint(this, _Player, 10.f, FLinearColor::Green, 100.f);
	UKismetSystemLibrary::DrawDebugPoint(this, _End, 10.f, FLinearColor::Red, 100.f);
}

void UDungeonManager::GenerateProps()
{
	// Room 내부에서 구현되는 것
	// Rect Light, 중앙 램프
	for (int i = 0; i < RoomList.Num(); i++)
	{
		RoomList[i]->GenerateLevel();
	}

	// 통로의 옆벽에 횃불 세우기
	for(int i = 0; i < RoadList.Num(); i++)
	{
		if(i % 3 == 0)
		{
			RoadList[i]->AddProp(LevelDataAsset->LeftWallLamp, TEXT("WallLamp"), true);
		}
	}

	// 방의 벽에 횟불 만들기
	for(int i = 0; i < RoomList.Num(); i++)
	{
		int _Count = 0;
		for(int j = 0; j < RoomList[i]->Cells.Num(); j++)
		{
			int _WallCount = RoomList[i]->Cells[j]->bLeftWall + RoomList[i]->Cells[j]->bTopWall + RoomList[i]->Cells[j]->bRightWall + RoomList[i]->Cells[j]->bBottomWall;
			if(_WallCount == 1)
			{
				if(_Count % 3 == 0)
				{
					RoomList[i]->Cells[j]->AddProp(LevelDataAsset->LeftWallLamp, TEXT("WallLamp"), true);
				}
				_Count++;
			}
		}
	}
	
	// 방에 기둥 세우기(Obstacle)
	for(int i = 0; i < RoomList.Num(); i++)
	{
		int _XStart = RoomList[i]->Rect->X;
		int _XEnd = _XStart + RoomList[i]->Rect->Width - 1;
		int _YStart = RoomList[i]->Rect->Y;
		int _YEnd = _YStart + RoomList[i]->Rect->Height - 1;
		int _XCount = RoomList[i]->Rect->Width % 2 == 1 ? 2 : 3;
		int _YCount = RoomList[i]->Rect->Height % 2 == 1 ? 2 : 3;

		for(int y = _YStart + 1; y <= _YEnd - 1; y++)
		{
			if((y - _YStart) % _YCount == 1)
			{
				for(int x = _XStart + 1; x <= _XEnd - 1; x++)
				{
					if((x - _XStart) % _XCount == 1)
					{
						UDungeonCell* _Cell = CellMap.FindRef(FVector2D(x, y));
						_Cell->AddProp(LevelDataAsset->Pillar, TEXT("Pillar"), false);
					}
				}
			}
		}
	}
	
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
			
			// 방(Room) 내의 랜덤 Cell 선택하기
			TArray<UDungeonCell*> _RandCells;
			while (_RandCells.Num() < _Count)
			{
				_RandCells.AddUnique(RoomList[i]->Cells[FMath::RandRange(0, RoomList[i]->Cells.Num() - 1)]);
			}
			
			// 몬스터 스폰하기
			for(int j = 0; j < _Count; j++)
			{
				// 랜덤 몬스터 스폰
				TSubclassOf<AMonsterGeneralCharacter> _MonsterClass = LevelDataAsset->RoomMonsters[FMath::RandRange(0, LevelDataAsset->RoomMonsters.Num() - 1)];
				FActorSpawnParameters _SpawnParams;
				_SpawnParams.OverrideLevel = GetWorld()->GetCurrentLevel();
				_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				_SpawnParams.bDeferConstruction = false;
				
				AMonsterGeneralCharacter* _Monster = GetWorld()->SpawnActor<AMonsterGeneralCharacter>(_MonsterClass, _RandCells[j]->Location + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator, _SpawnParams);

				// Room에 몬스터 등록하기
				_RandCells[j]->Room->Monsters.Add(_Monster);
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
	PlayerController = UGameplayStatics::GetPlayerController(GetOuter(), 0);
	FInputModeGameOnly _GameInputMode;
	PlayerController->SetInputMode(_GameInputMode);
	PlayerController->SetShowMouseCursor(false);
	// 플레이어 Pawn으로 Blend 시작
	PlayerController->SetViewTargetWithBlend(Player, 3.f);
	
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

	// Directional Light
	DirectionalLight->Destroy();
}

void UDungeonManager::UpdatePlayerLocation(APlayableCharacter* _Player)
{
	// 플레이어가 현재 있는 Cell 찾기
	UDungeonCell* _PlayerCell = FindClosestCell(_Player->GetActorLocation());
	if(_PlayerCell == nullptr)
	{
		LOGTEXT_ERROR(TEXT("!!!오류: 플레이어 Cell 찾기 실패"));
		return;
	}

	
	//LOGTEXT_LOG(TEXT("현재 플레이어 Cell:%s, Room?%d"), *_PlayerCell->Matrix.ToString(), _PlayerCell->bRoom);

	// 플레이어 방 입장
	if(_PlayerCell->bRoom && _PlayerCell->Room != nullptr)
	{
		_PlayerCell->Room->PlayerEnter(_Player);
	}
}

UDungeonCell* UDungeonManager::FindClosestCell(FVector _Location)
{
	_Location.Z = 0.f;
	UDungeonCell* _Closest = nullptr;
	float _Distance = 10000000000.f;
	for(int i = 0; i < YSize; i++)
	{
		for(int j = 0; j < XSize; j++)
		{
			UDungeonCell* _Cell = CellMap.FindRef(FVector2D(j, i));
			
			if(_Cell != nullptr)
			{
				float _CurDist = FVector::Distance(_Cell->Location, _Location);
				if(_CurDist < _Distance)
				{
					_Closest = _Cell;
					_Distance = _CurDist;
				}
			}
		}
	}

	return _Closest;
}



