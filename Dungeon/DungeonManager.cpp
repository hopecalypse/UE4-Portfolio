// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/DungeonManager.h"

#include "DungeonCell.h"
#include "DungeonDataAsset.h"
#include "DungeonRoom.h"
#include "ObserverPawn.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "PortFolio.h"
#include "RoomCollider.h"
#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"
#include "Core/GameManagerInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/PathManager.h"
#include "Monster/BossMonsterBase.h"
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

	static ConstructorHelpers::FClassFinder<UUserWidget> _ClearScreenClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_ClearScreen.WBP_ClearScreen_C'"));
	if(_ClearScreenClass.Succeeded())
		ClearScreenClass = _ClearScreenClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> _GameOverClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_GameOverScreen.WBP_GameOverScreen_C'"));
	if(_GameOverClass.Succeeded())
		GameOverScreen = _GameOverClass.Class;
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
	// _Rect ?????? ?????? Cell??? ?????????
	// ??????~???
	for(int y = _Rect.Y; y < _Rect.Y + _Rect.Height; y++)
	{
		for(int x = _Rect.X; x < _Rect.X + _Rect.Width; x++)
		{
			UDungeonCell* _FoundCell = FindCell(FMatrix2D(x, y));
			if(_FoundCell != nullptr)
				_CellArray.Add(_FoundCell);
		}
	}
	return _CellArray;
}

UDungeonCell* UDungeonManager::FindCell(FMatrix2D _Mat)
{
	UDungeonCell* _Cell = nullptr;
	for(int i = 0; i < CellList.Num(); i++)
	{
		if(CellList[i]->Matrix == _Mat)
		{
			_Cell = CellList[i];
			break;
		}
	}
	return _Cell;
}

void UDungeonManager::GenerateGrid(int _Width, int _Height)
{
	// ?????? ??????
	XSize = _Width;
	YSize = _Height;
	
	// Tester ?????? ??????
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for(int i = 0; i < _Height; i++)
	{
		for(int j = 0; j < _Width; j++)
		{
			UDungeonCell* _GenCell = NewObject<UDungeonCell>(this);
			_GenCell->InitCell(j, i);
			//CellMap.Add(FMatrix2D(j, i), _GenCell);
			CellList.Add(_GenCell);
			
			AActor* _Visualizer = GetWorld()->SpawnActor<AActor>(LevelDataAsset->TesterFloor, _GenCell->Location, FRotator::ZeroRotator, _SpawnParams);
			_GenCell->Visualizer = _Visualizer;
			_GenCell->bRoom = false;
			_GenCell->bRoad = false;
			
			// 16?????? PathNode ??????
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
					
					// ?????? Cell??? ????????? PathNode??? ??????
					FVector2D _CellMatrix = FVector2D(l, k);
					_GenCell->PathNodes.Add(_CellMatrix, _PathNode);
					_PathNode->Cell = _GenCell;
				}
			}
		}
	}

	// Root ?????? ??????
	RootNode = new FTreeNode(0, 0, _Width, _Height);
	RootNode->bIsLeaf = false;

	// ????????? ???????????? ?????? ??????
	APawn* _ObserverPlayer = UGameplayStatics::GetPlayerPawn(GetOuter(), 0);
	float _X = _Height * 300.f;
	float _Y = _Width * 300.f;
	_ObserverPlayer->SetActorLocation(FVector(_X, _Y, FMath::Max(_Width, _Height) * 700.f));
	_ObserverPlayer->SetActorRotation(FRotator(-90.f, 0.f, 0.f));
	ObserverOriginLocation = _ObserverPlayer->GetActorLocation();
	ObserverOriginRotation = _ObserverPlayer->GetActorRotation();

	// ????????? ????????? ??????
	DirectionalLight = GetWorld()->SpawnActor<AActor>(LevelDataAsset->DirectionalLight, FVector::ZeroVector, FRotator::ZeroRotator, _SpawnParams);
}

void UDungeonManager::StartSplitTree(int _Count)
{
	// Depth Array ?????????
	for(int i = 0; i <= _Count; i++)
	{
		TArray<FTreeNode*> _Arr;
		TreeDepthLists.Add(_Arr);
	}
	// Count?????? ?????? ?????? ??????
	SplitTree(RootNode, _Count);
}

void UDungeonManager::SplitTree(FTreeNode* _TreeNode, int _Depth)
{
	// ????????? ?????? ????????? 3?????? ????????? ?????? ?????? Leaf??? ????????? ??????
	if(_Depth == 0 || _TreeNode->Rect.Width <= 3 || _TreeNode->Rect.Height <= 3)
	{
		_TreeNode->bIsLeaf = true;
		TreeDepthLists[TreeDepthLists.Num() - _Depth - 1].Add(_TreeNode);
		return;
	}
	
	// ?????? ????????????
	_TreeNode->bIsLeaf = false;
	TreeDepthLists[TreeDepthLists.Num() - _Depth - 1].Add(_TreeNode);

	// ???????????? ?????? ?????? ??????
	float _RandRatio = FMath::RandRange(0.4f, 0.6f);

	// ?????? ??????
	if(_TreeNode->Rect.Width >= _TreeNode->Rect.Height)
	{
		int _Split = FMath::RoundToInt(_TreeNode->Rect.Width * _RandRatio);	
		_TreeNode->Left = new FTreeNode(_TreeNode->Rect.X, _TreeNode->Rect.Y, _Split, _TreeNode->Rect.Height);
		_TreeNode->Right = new FTreeNode(_TreeNode->Rect.X + _Split, _TreeNode->Rect.Y, _TreeNode->Rect.Width - _Split, _TreeNode->Rect.Height);
	}
	// ?????? ??????
	else
	{
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
		UKismetSystemLibrary::DrawDebugLine(this,  _BottomStart, _BottomEnd, _Color, 30.f, 100.f);
		
		// Top
		FVector _TopStart = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, -280.f + _TreeNode->Rect.X * 600.f, 10.f * _Depth);
		FVector _TopEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, -280.f + (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _TopStart, _TopEnd, _Color, 30.f, 100.f);
		
		// Left
		FVector _LeftStart = FVector(_TreeNode->Rect.Y * 600.f - 280.f, _TreeNode->Rect.X * 600.f - 280.f, 10.f * _Depth);
		FVector _LeftEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, _TreeNode->Rect.X * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _LeftStart, _LeftEnd, _Color, 30.f, 100.f);
		
		// Right
		FVector _RightStart = FVector(_TreeNode->Rect.Y * 600.f - 280.f, (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, 10.f * _Depth);
		FVector _RightEnd = FVector((_TreeNode->Rect.Y + _TreeNode->Rect.Height) * 600.f - 280.f, (_TreeNode->Rect.X + _TreeNode->Rect.Width) * 600.f - 280.f, 10.f * _Depth);
		UKismetSystemLibrary::DrawDebugLine(this, _RightStart, _RightEnd, _Color, 30.f, 100.f);
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
		// ?????? ??? ?????? ?????????(?????? 2 ~ ?????? (??????-1))
		int _Width = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Width / 2) + 1, _TreeNode->Rect.Width * 2 / 3);
		_Width = FMath::Clamp(_Width, 2, _TreeNode->Rect.Width - 1);
		int _Height = FMath::RandRange(FMath::RoundToInt(_TreeNode->Rect.Height / 2) + 1, _TreeNode->Rect.Height * 2 / 3);
		_Height = FMath::Clamp(_Height, 2, _TreeNode->Rect.Width - 1);

		// ?????? ??????
		int _X = _TreeNode->Rect.X + FMath::RandRange(0, _TreeNode->Rect.Width - _Width);
		int _Y = _TreeNode->Rect.Y + FMath::RandRange(0, _TreeNode->Rect.Height - _Height);

		// Room ???????????? ????????????
		UDungeonRoom* _Room = NewObject<UDungeonRoom>(GetWorld());
		
		// ???????????? Cell??? ????????? ??????
		TArray<UDungeonCell*> _RoomCells = GetCellsInside(FRect(_X, _Y, _Width, _Height));
		for(int i = 0; i < _RoomCells.Num(); i++)
		{
			_RoomCells[i]->ClearVisualizer();
			_RoomCells[i]->bRoom = true;
			_RoomCells[i]->Room = _Room;
			
			_Room->Cells.Add(_RoomCells[i]);
		}
		
		// Room??? Rect ??????
		FRect* _Rect = new FRect(_X, _Y, _Width, _Height);
		_Room->Rect = _Rect;
		//_Room->CenterCell = CellMap.FindRef(FMatrix2D(_X + FMath::DivideAndRoundNearest(_Width, 2), _Y + FMath::DivideAndRoundNearest(_Height, 2)));
		_Room->CenterCell = FindCell(FMatrix2D(_X + FMath::DivideAndRoundNearest(_Width, 2), _Y + FMath::DivideAndRoundNearest(_Height, 2)));

		// Room ???????????? ??????
		float _XStart = _X * 600.f - 300.f;
		float _XEnd = (_X + _Width) * 600.f - 300.f;
		float _YStart = _Y * 600.f - 300.f;
		float _YEnd = (_Y + _Height) * 600.f - 300.f;
		float _XCenter = (_XStart + _XEnd) / 2.f;
		float _YCenter = (_YStart + _YEnd) / 2.f;
		FActorSpawnParameters _SpawnParams;
		_SpawnParams.OverrideLevel = GetWorld()->GetCurrentLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ARoomCollider* _Collider = GetWorld()->SpawnActor<ARoomCollider>(ARoomCollider::StaticClass(), FVector(_YCenter, _XCenter, 100.f), FRotator::ZeroRotator, _SpawnParams);
		_Collider->ColliderBox->SetBoxExtent(FVector(_Height * 300.f, _Width * 300.f, 200.f));
		_Collider->Room = _Room;

		// ????????????
		// UKismetSystemLibrary::DrawDebugPoint(this, _Collider->GetActorLocation(), 10.f, FLinearColor::Yellow, 100.f);
		// UKismetSystemLibrary::DrawDebugBox(this, _Collider->GetActorLocation(), _Collider->ColliderBox->GetScaledBoxExtent(), FLinearColor::Red, FRotator::ZeroRotator, 100.f, 40.f);

		// Rect Light ??????
		AActor* _RectLight = GetOuter()->GetWorld()->SpawnActor<AActor>(LevelDataAsset->RoomRectLight, _Collider->GetActorLocation() + FVector(0.f, 0.f, 600.f), FRotator::ZeroRotator, _SpawnParams);
		URectLightComponent* _LightComp = _RectLight->FindComponentByClass<URectLightComponent>();
		
		_LightComp->SetSourceWidth(_Width * 600.f - 30.f);
		_LightComp->SetSourceHeight(_Height * 600.f - 300.f);
		

		RoomList.Add(_Room);
		LOGTEXT_LOG(TEXT("Room(%s) ?????????: (Cell??????:%d)"), *_Room->GetName(), _Room->Cells.Num());
		//UKismetSystemLibrary::DrawDebugCircle(this, _Room->CenterCell->Location, 100.f, 10.f, FLinearColor::Green, 100.f, 50.f);
	}

	else
	{
		GenerateRoom(_TreeNode->Left);
		GenerateRoom(_TreeNode->Right);
	}
}

bool UDungeonManager::IsNotRoom(FMatrix2D _Matrix)
{
	//UDungeonCell* _Cell = CellMap.FindRef(_Matrix);
	UDungeonCell* _Cell = FindCell(_Matrix);
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
	
	// Rect??? ?????? ~ ???????????? ??????
	FVector2D _LeftCenter = GetRectCenter(_TreeNode->Left->Rect);
	FVector2D _RightCenter = GetRectCenter(_TreeNode->Right->Rect);

	// ??????
	for(int i = FMath::Min(_LeftCenter.X, _RightCenter.X); i <= FMath::Max(_LeftCenter.X, _RightCenter.X); i++)
	{
		//UDungeonCell* _RoadCell = CellMap.FindRef(FMatrix2D(i, _LeftCenter.Y));
		UDungeonCell* _RoadCell = FindCell(FMatrix2D(i, _LeftCenter.Y));
		_RoadCell->bRoad = true;
		UKismetSystemLibrary::DrawDebugBox(this, _RoadCell->Location + FVector(0.f, 0.f, 100.f), FVector(280.f), FLinearColor::Blue, FRotator::ZeroRotator, 10.f);

		// Road ???????????? ??????
		RoadList.AddUnique(_RoadCell);
	}

	// ??????
	for(int i = FMath::Min(_LeftCenter.Y, _RightCenter.Y); i <= FMath::Max(_LeftCenter.Y, _RightCenter.Y); i++)
	{
		UDungeonCell* _RoadCell = FindCell(FMatrix2D(_LeftCenter.X, i));
		_RoadCell->bRoad = true;
		UKismetSystemLibrary::DrawDebugBox(this, _RoadCell->Location + FVector(0.f, 0.f, 100.f), FVector(280.f), FLinearColor::Red, FRotator::ZeroRotator, 10.f);

		// Road ???????????? ??????
		RoadList.AddUnique(_RoadCell);
	}
	
	GenerateRoad(_TreeNode->Left);
	GenerateRoad(_TreeNode->Right);
}

FVector2D UDungeonManager::GetRectCenter(FRect _Rect)
{
	return FVector2D(_Rect.X + FMath::RoundToInt(_Rect.Width / 2), _Rect.Y + FMath::RoundToInt(_Rect.Height / 2));
}

bool UDungeonManager::IsNotRoad(FMatrix2D _Matrix)
{
	UDungeonCell* _Cell = FindCell(_Matrix);
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
	// Bottom Left(Min), Top Right(Max) ????????????
	UDungeonRoom* _BottomLeft = RoomList[0];
	UDungeonRoom* _TopLeft = RoomList[0];
	for(int i = 1; i < RoomList.Num(); i++)
	{
		int _Cur = RoomList[i]->CenterCell->Matrix.X + RoomList[i]->CenterCell->Matrix.Y;
		
		// ??????
		if(_Cur < (_BottomLeft->CenterCell->Matrix.X +_BottomLeft->CenterCell->Matrix.Y))
			_BottomLeft = RoomList[i];
		// ??????
		if(_Cur > (_TopLeft->CenterCell->Matrix.X + _TopLeft->CenterCell->Matrix.Y))
			_TopLeft = RoomList[i];
	}

	// ??? ?????? ??? ??? ?????? ??????
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
	// Room ???????????? ???????????? ???
	// Rect Light, ?????? ??????
	for (int i = 0; i < RoomList.Num(); i++)
	{
		RoomList[i]->GenerateLevel();
	}

	// ????????? ????????? ?????? ?????????
	for(int i = 0; i < RoadList.Num(); i++)
	{
		if(i % 3 == 0)
		{
			RoadList[i]->AddProp(LevelDataAsset->LeftWallLamp, TEXT("WallLamp"), true);
		}
	}

	// ?????? ?????? ?????? ?????????
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
	
	// ?????? ?????? ?????????(Obstacle)
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
			bool _YFlag = false;
			if((y - _YStart) % _YCount == 1)
				_YFlag = true;
			// if(_YCount == 3 && ((y - _YStart) % _YCount == 2) && y == _YEnd - 1)
			// 	_YFlag = true;

			if(_YFlag)
			{
				for(int x = _XStart + 1; x <= _XEnd - 1; x++)
				{
					if((x - _XStart) % _XCount == 1)
					{
						UDungeonCell* _Cell = FindCell(FMatrix2D(x, y));
						if(_Cell->Room != nullptr)
						{
							if(_Cell->Room->CenterCell != _Cell)
								_Cell->AddProp(LevelDataAsset->Pillar, TEXT("Pillar"), false);
						}
					}
				}
			}
		}
	}
	
}

void UDungeonManager::GenerateMonster()
{
	// ???????????? ?????????, ???????????? ?????? ?????? ????????? ?????? ????????????
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(!RoomList[i]->bPlayerStart && !RoomList[i]->bBossRoom)
		{
			// ????????? ??????
			int _Count = FMath::RandRange(3, 6);
			
			// ???(Room) ?????? ?????? Cell ????????????
			TArray<UDungeonCell*> _RandCells;
			while (_RandCells.Num() < _Count)
				_RandCells.AddUnique(RoomList[i]->Cells[FMath::RandRange(0, RoomList[i]->Cells.Num() - 1)]);
			
			// ????????? ????????????
			for(int j = 0; j < _Count; j++)
			{
				// ?????? ????????? ??????
				TSubclassOf<AMonsterGeneralCharacter> _MonsterClass = LevelDataAsset->RoomMonsters[FMath::RandRange(0, LevelDataAsset->RoomMonsters.Num() - 1)];
				FActorSpawnParameters _SpawnParams;
				_SpawnParams.OverrideLevel = GetWorld()->GetCurrentLevel();
				_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				AMonsterGeneralCharacter* _Monster = GetWorld()->SpawnActor<AMonsterGeneralCharacter>(_MonsterClass, _RandCells[j]->Location + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator, _SpawnParams);

				// Room??? ????????? ????????????
				RoomList[i]->Monsters.Add(_Monster);
				//_RandCells[j]->Room->Monsters.Add(_Monster);
			}
		}
	}

	// ????????? ??????????????? ??????
	FActorSpawnParameters _SpawnParams;
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABossMonsterBase* _Boss = GetWorld()->SpawnActor<ABossMonsterBase>(LevelDataAsset->BossMonster, BossRoom->CenterCell->Location + FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, _SpawnParams);
	BossRoom->Monsters.Add(_Boss);
	_Boss->SpawnLocation = _Boss->GetActorLocation();

	// ?????? ????????????
	FRotator _Rotation = UKismetMathLibrary::FindLookAtRotation(_Boss->GetActorLocation(), BossRoom->DoorCell->Location);
	_Rotation.Pitch = 0.f;
	_Rotation.Roll = 0.f;
	_Boss->SetActorRotation(_Rotation);
}

void UDungeonManager::UpdateCells()
{
	for (auto _Cell : CellList)
	{
		_Cell->GenerateLevel();
	}
}

void UDungeonManager::StartGame()
{
	// ???????????? HUD ??????
	PlayerHUD = Cast<UPlayerHUD>(CreateWidget(GetOuter()->GetWorld(), PlayerHUDBP));
	PlayerHUD->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->AddToViewport();
	PlayerHUD->SetVisibility(ESlateVisibility::Hidden);
	
	// ???????????? ????????????
	ObserverPawn = UGameplayStatics::GetPlayerPawn(GetOuter(), 0);
	Cast<AObserverPawn>(ObserverPawn)->bFreeze = true;
	TSubclassOf<APlayableCharacter> _SelPlayerClass = Cast<UGameManagerInstance>(UGameplayStatics::GetGameInstance(GetOuter()))->GetSelectedClassBP();
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetOuter()->GetWorld()->GetCurrentLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Player = GetOuter()->GetWorld()->SpawnActor<APlayableCharacter>(_SelPlayerClass, PlayerStartRoom->CenterCell->Location + FVector(0.f, 0.f, 88.f), FRotator::ZeroRotator, _SpawnParams);
	Player->SetPlayerHUD(PlayerHUD);
	
	// InputMode ??????
	PlayerController = UGameplayStatics::GetPlayerController(GetOuter(), 0);
	FInputModeGameOnly _GameInputMode;
	PlayerController->SetInputMode(_GameInputMode);
	PlayerController->SetShowMouseCursor(false);
	// ???????????? Pawn?????? Blend ??????
	PlayerController->SetViewTargetWithBlend(Player, 3.f);
	
	// ????????? ??????
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, TEXT("OnEndBlendToPlayer"));

	GetOuter()->GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, 3.f, false);
}

void UDungeonManager::OnEndBlendToPlayer()
{
	// ObserverPawn ??????
	ObserverPawn->Destroy();
	// ????????????
	APlayerController* _PlayerController = UGameplayStatics::GetPlayerController(GetOuter(), 0);
	_PlayerController->Possess(Player);
	// Debug ?????????
	UKismetSystemLibrary::FlushPersistentDebugLines(this);

	// Directional Light
	DirectionalLight->Destroy();
	PlayerHUD->SetVisibility(ESlateVisibility::Visible);
}

void UDungeonManager::UpdatePlayerLocation(APlayableCharacter* _Player)
{
	// ??????????????? ?????? ?????? Cell ??????
	UDungeonCell* _PlayerCell = FindClosestCell(_Player->GetActorLocation());
	if(_PlayerCell == nullptr)
		return;

	if(_PlayerCell->Room == nullptr)
		return;

	// ???????????? ??? ??????
	if(_PlayerCell->bRoom && _PlayerCell->Room != nullptr)
	{
		_PlayerCell->Room->PlayerEnter(_Player);
	}
}

UDungeonCell* UDungeonManager::FindClosestCell(FVector _Location)
{
	_Location.Z = 0.f;
	UDungeonCell* _Closest = CellList[0];
	float _Distance = 10000000000.f;
	for(int i = 0; i < YSize; i++)
	{
		for(int j = 0; j < XSize; j++)
		{
			UDungeonCell* _Cell = FindCell(FMatrix2D(j, i));
			
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

void UDungeonManager::ClearBoss()
{
	FInputModeUIOnly _UIMode;
	PlayerController->SetInputMode(_UIMode);
	PlayerController->SetShowMouseCursor(true);

	UUserWidget* _ClearScreenWidget = CreateWidget(GetWorld(), ClearScreenClass);
	_ClearScreenWidget->AddToViewport();
}

void UDungeonManager::GameOver()
{
	FInputModeUIOnly _UIMode;
	PlayerController->SetInputMode(_UIMode);
	PlayerController->SetShowMouseCursor(true);
	
	UUserWidget* _GameOverWidget = CreateWidget(GetWorld(), GameOverScreen);
	_GameOverWidget->AddToViewport();
	
}

