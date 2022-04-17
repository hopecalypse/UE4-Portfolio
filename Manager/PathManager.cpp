// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PathManager.h"

#include "PortFolio.h"
#include "Dungeon/DungeonManager.h"

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
	for(auto& _Iter : PathNodeMap)
	{
		_Iter.Value->F = 0;
		_Iter.Value->G = 0;
		_Iter.Value->H = 0;
		_Iter.Value->Parent = nullptr;
	}

	FPathNode* _StartNode = FindCloseNode(_Start);
	FPathNode* _EndNode = FindCloseNode(_End);

	//LOGTEXT_ERROR(TEXT("길찾기 시작:Node(%s)위치(%s), 목표:Node(%s)위치(%s)"), *_StartNode->Matrix.ToString(), *_Start.ToString(), *_EndNode->Matrix.ToString(), *_End.ToString());

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
	// 현재 노드 = 출발
	_OpenList.Add(_StartNode);

	FPathNode* _Current = nullptr;
	
	// 목표물까지 반복
	while (_OpenList.Num() > 0)
	{
		// 현재 노드(F값이 가장 작은 값 OpenList에서 꺼내기)
		_Current = PopOpenList(&_OpenList);

		//LOGTEXT_LOG(TEXT("길찾기중: 현재 노드=%s, 현재 OpenList갯수=%s"), *_Current->Matrix.ToString(), *FString::FromInt(_OpenList.Num()));
		
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
			_OpenList.AddUnique(_CheckNode);
		}
	}
	
	// 도착 노드로부터 거슬러 올라가는 게 경로(Pop으로 쓸 것)
	TArray<FVector> _PathArray;
	while (_Current != nullptr)
	{
		_PathArray.Add(_Current->Location);
		_Current = _Current->Parent;
	}
	return _PathArray;
	
}

FPathNode* UPathManager::FindCloseNode(FVector _Location)
{
	_Location.Z = 0;
	FPathNode* _Close = PathNodeMap.FindRef(FVector2D(0, 0));
	for (auto& _Iter : PathNodeMap)
	{
		if(FVector::Distance(_Location, _Iter.Value->Location) < FVector::Distance(_Location, _Close->Location))
			_Close = _Iter.Value;
	}

	return _Close;
}

FPathNode* UPathManager::PopOpenList(TArray<FPathNode*>* _OpenList)
{
	//LOGTEXT_LOG(TEXT("길찾기중: 오픈리스트 Pop 호출. 현재갯수:%d"), _OpenList->Num());
	// OpenList에서 F값(코스트)가 가장 작은 노드 꺼내기
	if(_OpenList->Num() < 1)
		return nullptr;
	if(_OpenList->Num() == 1)
	{
		FPathNode* _Node = (*_OpenList)[0];
		_OpenList->Empty();
		return _Node;
	}
	
	FPathNode* _Lowest = (*_OpenList)[0];
	for(int i = 1; i < _OpenList->Num(); i++)
	{
		if((*_OpenList)[i]->F < _Lowest->F)
			_Lowest = (*_OpenList)[i];
	}
	_OpenList->Remove(_Lowest);
	return _Lowest;
}

