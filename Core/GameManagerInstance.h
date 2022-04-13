// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManagerInstance.generated.h"

class APlayableCharacter;
class UDungeonManager;
enum class EPlayerClass : uint8;
/**
 *			게임 매니저들을 관리하는 게임 인스턴스
 */
UCLASS()
class PORTFOLIO_API UGameManagerInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGameManagerInstance();
	// 시스템 함수 -> 매니저들 조정
public:
    virtual void Init() override;
	virtual void Shutdown() override;


	// ! 플레이어 정보 관련 데이터
	// 플레이어 직업
private:
	EPlayerClass SelectedPlayerClass;
public:
	bool HasSelectedClass;
	void SelectPlayerClass(EPlayerClass _SelectClass);
	EPlayerClass GetPlayerClass() { return SelectedPlayerClass; }

public:
	TSubclassOf<APlayableCharacter> WarriorBP;
	TSubclassOf<APlayableCharacter> MageBP;
	TSubclassOf<APlayableCharacter> GunnerBP;
	TSubclassOf<APlayableCharacter> GetSelectedClassBP();

	// 생성자, 소멸자(각 매니저들 Destory 호출)
//public:
//	UGameManagerInstance();
//	~UGameManagerInstance();
};
