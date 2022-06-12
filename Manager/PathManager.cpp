// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PathManager.h"

#include "PortFolio.h"
#include "Dungeon/DungeonManager.h"
#include "Kismet/KismetSystemLibrary.h"

UPathManager* UPathManager::ManagerInstance = nullptr;

void UPathManager::GenerateInst(UObject* _GameInstance)
{
	if(ManagerInstance == nullptr)
	{
		ManagerInstance = NewObject<UPathManager>(_GameInstance);
		ManagerInstance->AddToRoot();
	}
}

UPathManager* UPathManager::Instance()
{
	return ManagerInstance;
}

void UPathManager::DestroyManager()
{
	if(ManagerInstance != nullptr)
	{
		ManagerInstance->ConditionalBeginDestroy();
		ManagerInstance = nullptr;
	}
}

TArray<FVector> UPathManager::FindPath(FVector _Start, FVector _End)
{
	// 노드들을 초기화하기
	for(int i = 0; i < PathNodeArray.Num(); i++)
	{
		PathNodeArray[i]->F = 0.f;
		PathNodeArray[i]->G = 0.f;
		PathNodeArray[i]->H = 0.f;
		PathNodeArray[i]->Parent = nullptr;
	}

	FPathNode* _StartNode = FindCloseNode(_Start);
	FPathNode* _EndNode = FindCloseNode(_End);

	TArray<FPathNode*> _OpenList;
	TArray<FPathNode*> _CloseList;
	
	// 8방향
	// TArray<FVector2D> _Dir = {FVector2D(0, 1), FVector2D(1, 0), FVector2D(0, -1), FVector2D(-1, 0),
	// 							FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, -1), FVector2D(-1, 1)};
	TArray<FVector2D> _Dir;
	_Dir.Add(FVector2D(0 , 1));
	_Dir.Add(FVector2D(1, 0));
	_Dir.Add(FVector2D(-1, 0));
	_Dir.Add(FVector2D(0,  -1));
	_Dir.Add(FVector2D(1, 1));
	_Dir.Add(FVector2D(1, -1));
	_Dir.Add(FVector2D(-1, 1));
	_Dir.Add(FVector2D(-1, -1));

	// 출발 노드부터 시작
	_OpenList.Add(_StartNode);

	FPathNode* _Current = nullptr;
	
	// 목표물까지 반복
	while (_OpenList.Num() > 0)
	{
		// 현재 노드(F값이 가장 작은 값 OpenList에서 꺼내기)
		_Current = PopOpenList(&_OpenList);
		
		// 도착 여부
		if(_Current == _EndNode)
			break;
		
		_CloseList.AddUnique(_Current);
		
		// 8방향 노드들을 계산하기(F, G, H)
		for(int i = 0; i < 8; i++)
		{
			FPathNode* _CheckNode = PathNodeMap.FindRef(_Current->Matrix + _Dir[i]);

			// 없는 경우
			if(_CheckNode == nullptr)
				continue;
			// 막혀있을 경우->Close리스트에 추가
			if(_CheckNode->bObstacle)
			{
				_CloseList.AddUnique(_CheckNode);
				continue;
			}
			// 이미 Close인경우
			if(_CloseList.Contains(_CheckNode))
				continue;

			// 이미 OpenList에 있을 때-> 갱신할지 여부
			if(_OpenList.Contains(_CheckNode))
			{
				// 새로운 경로보다 만약 기존 OpenNode가 더 좋다면-> 그냥 넘어가기(닫기x)
				int _NewG = _Current->G + (i >= 4 ? 14 : 10);
				if(_CheckNode->G < _NewG)
					continue;
			}
			// 새로운 노드일 때
			// G값: G + 직선10 대각선14,  H값: 거리 * 10
			_CheckNode->G = _Current->G + (i >= 4 ? 14 : 10);
			_CheckNode->H = (FMath::Abs(_EndNode->Matrix.X - _CheckNode->Matrix.X) * 10) + (FMath::Abs(_EndNode->Matrix.Y - _CheckNode->Matrix.Y) * 10);
			_CheckNode->F = _CheckNode->G + _CheckNode->H;
			_CheckNode->Parent = _Current;

			// 리스트 구조 Heap으로 변경
			//_OpenList.AddUnique(_CheckNode);
			_OpenList.HeapPush(_CheckNode, FPathNodePriority());
		}
	}
	
	// 도착 노드로부터 거슬러 올라가며 경로에 추가
	TArray<FVector> _PathArray;
	while (_Current != nullptr)
	{
		_PathArray.Add(_Current->Location);
		_Current = _Current->Parent;
	}

	// 시각화
	for (int i = 0; i < _PathArray.Num() - 1; i++)
	{
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), _PathArray[i], 10.f, FLinearColor::Red, 100.f);
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), _PathArray[i + 1], 10.f, FLinearColor::Red, 100.f);
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), _PathArray[i], _PathArray[i + 1], FLinearColor::Green, 3.f, 5.f);
	}

	// 경로 지점(FVector)들 반환
	return _PathArray;
}

FPathNode* UPathManager::FindCloseNode(FVector _Location)
{
	// Input 위치에서 가장 가까운 PathNode 찾기
	_Location.Z = 0;
	FPathNode* _Close = nullptr;
	float _Dist = 100000000.f;
	for(int i = 0; i < PathNodeArray.Num(); i++)
	{
		if(FVector::Distance(_Location, PathNodeArray[i]->Location) < _Dist)
		{
			_Close = PathNodeArray[i];
			_Dist = FVector::Distance(_Location, PathNodeArray[i]->Location);
		}
	}
	if(_Close == nullptr)
		LOGTEXT_ERROR(TEXT("!!!Error: FindNode Null!!!"));
	return _Close;
}

FPathNode* UPathManager::PopOpenList(TArray<FPathNode*>* _OpenList)
{
	// OpenList에서 F(Cost)가 가장 작은 노드 꺼내기
	if(_OpenList->Num() < 1)
		return nullptr;
	if(_OpenList->Num() == 1)
		return _OpenList->Pop();

	// 기존 구조: 원소 모두 순회하며 가장 작은 값 찾기
	// FPathNode* _Lowest = (*_OpenList)[0];
	// for(int i = 1; i < _OpenList->Num(); i++)
	// {
	// 	if((*_OpenList)[i]->F < _Lowest->F)
	// 		_Lowest = (*_OpenList)[i];
	// }
	// _OpenList->Remove(_Lowest);
	//return _Lowest;

	// 변경 이후: Heap 사용
	FPathNode* _Min;
	_OpenList->HeapPop(_Min, FPathNodePriority());
	return _Min;
}

