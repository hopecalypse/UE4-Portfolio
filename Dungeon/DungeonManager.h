// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonManager.generated.h"

class UDungeonDataAsset;
class UDungeonCell;
/**
 * 
 */

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
	{
		
	}
	FTreeNode(int _X, int _Y, int _Width, int _Height)
	{
		Parent = nullptr;
		Left = nullptr;
		Right = nullptr;
		Rect = FRect(_X, _Y, _Width, _Height);
	}
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
	TMap<FVector2D, UDungeonCell*> CellList;
	TArray<FTreeNode*> TreeNodeList;
	TArray<TArray<FTreeNode*>> TreeDepthLists;

	// 0. 그리드 생성하기
public:
	void GenerateGrid(int _Width, int Height);

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
	bool IsNotRoom(FVector2D _Matrix);

	// 3. Cell 레벨 생성하기
public:
	void UpdateCells();
	
	void Test();
	
};
