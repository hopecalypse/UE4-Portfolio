// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonManager.generated.h"

class UClearScreen;
class APlayableCharacter;
class UPlayerHUD;
class UDungeonRoom;
class UDungeonDataAsset;
class UDungeonCell;
/**
 * 
 */

USTRUCT(Atomic)
struct FMatrix2D
{
	GENERATED_USTRUCT_BODY()
public:
	int X;
	int Y;

	FMatrix2D()
		:X(0)
		,Y(0)
	{}
	FMatrix2D(int _X, int _Y)
		:X(_X)
		,Y(_Y)
	{}
	FMatrix2D operator+(const FMatrix2D& _Other) const
	{
		return FMatrix2D(X + _Other.X, Y + _Other.Y);
	}
	bool operator==(const FMatrix2D _Other) const
	{
		if(X == _Other.X && Y == _Other.Y)
			return true;
		else
			return false;
	}
};

USTRUCT(Atomic)
struct FRect
{
	GENERATED_USTRUCT_BODY()
public:
	int X;
	int Y;
	int Width;
	int Height;

	FRect()
	{
		
	}
	FRect(int _X, int _Y, int _Width, int _Height)
	{
		X = _X;
		Y = _Y;
		Width = _Width;
		Height = _Height;
	}
};


USTRUCT(Atomic)
struct FTreeNode
{
	GENERATED_USTRUCT_BODY()
public:
	FTreeNode* Parent;
	FTreeNode* Left;
	FTreeNode* Right;
	FRect Rect;
	FRect Room;
	bool bIsLeaf;

	FTreeNode()
	{ }
	FTreeNode(int _X, int _Y, int _Width, int _Height)
		:Parent(nullptr), Left(nullptr), Right(nullptr)
		,Rect(FRect(_X, _Y, _Width, _Height))
	{ }
};


UCLASS(Blueprintable)
class PORTFOLIO_API UDungeonManager : public UObject
{
	GENERATED_BODY()

public:
	UDungeonManager();
private:
	static UDungeonManager* ManagerInstance;
public:
	static void GenerateInst(UObject* _GameInstance);
	static UDungeonManager* Instance();
	static void DestroyManager();

	// 플레이어 필요 에셋
public:
	TSubclassOf<UUserWidget> PlayerHUDBP;
	UPlayerHUD* PlayerHUD;

	// 던전 Level BP 에셋
private:
	UDungeonDataAsset* LevelDataAsset;
public:
	UDungeonDataAsset* GetLevelData() { return LevelDataAsset; }
	TArray<UDungeonCell*> GetCellsInside(FRect _Rect);

	// 구현
public:
	int Phase;
	FTreeNode* RootNode;
	int XSize;
	int YSize;
	//TMap<FMatrix2D, UDungeonCell*> CellMap;
	TArray<UDungeonCell*> CellList;
	UDungeonCell* FindCell(FMatrix2D _Mat);
	TArray<FTreeNode*> TreeNodeList;
	TArray<TArray<FTreeNode*>> TreeDepthLists;
	TArray<UDungeonRoom*> RoomList;
	TArray<UDungeonCell*> RoadList;
	UDungeonRoom* PlayerStartRoom;
	UDungeonRoom* BossRoom;
	APawn* ObserverPawn;
	AActor* DirectionalLight;
	FVector ObserverOriginLocation;
	FRotator ObserverOriginRotation;

	// 0. 그리드 생성하기
public:
	void GenerateGrid(int _Width, int Height);
	// Cell 레벨 생성하기
	void UpdateCells();

	// 1. 분할하기
public:
	void StartSplitTree(int _Count);
private:
	UMaterial* RedMaterial;
	void SplitTree(FTreeNode* _TreeNode, int _Depth);
public:
	bool VisualizeTree(int _Depth);

	// 2. 방 생성하기
public:
	void StartGenerateRoom();
	void GenerateRoom(FTreeNode* _TreeNode);
	bool IsNotRoom(FMatrix2D _Matrix);

	// 4. 길 연결하기
public:
	void StartGenerateRoad();
	void GenerateRoad(FTreeNode* _TreeNode);
	FVector2D GetRectCenter(FRect _Rect);
	bool IsNotRoad(FMatrix2D _Matrix);

	// 5. Room들 분류, 진행 방향 설정
public:
	void SortRooms();

	// 5-1. 길찾기 테스트
public:
	void TestPathFinding();

	// 6. 소품 생성
public:
	void GenerateProps();

	// 7. 몬스터 생성
public:
	void GenerateMonster();
	
	// 8. 플레이어 생성하기, 게임 시작
public:
	void StartGame();
	UFUNCTION()
	void OnEndBlendToPlayer();
	
	// 인게임 관련
public:
	APlayableCharacter* Player;
	APlayerController* PlayerController;
	void UpdatePlayerLocation(APlayableCharacter* _Player);
	UDungeonCell* FindClosestCell(FVector _Location);
	UDungeonRoom* PlayerRoom;

	// 게임 클리어시
public:
	TSubclassOf<UUserWidget> ClearScreenClass;
	void ClearBoss();

	// 게임오버 UI
public:
	TSubclassOf<UUserWidget> GameOverScreen;
	void GameOver();
	
};
