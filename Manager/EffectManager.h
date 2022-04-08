// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EffectManager.generated.h"

class APoseEffectActor;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UEffectManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

private:
	static UEffectManager* ManagerInstance;

public:
	UEffectManager();
	static void GenerateInst(UObject* _GameInstance);
	static UEffectManager* Instance();
	static void DestroyManager();
	// Tick 관련
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	bool bIsOnRunning = false;
	
	// 플레이어 스킬 이펙트 관련

	// 플레이어 Pose Effect
public:
	UFUNCTION()
	void PoseEffectOnce(ACharacter* _Character, float _LastTime);
	void SetPoseEffectLoop(bool _Value, ACharacter* _Character = nullptr, float _Interval = 1.f, float _LastTime = 1.f);
	void TickPoseEffectLoop(float _DeltaTime);
	bool bIsOnPoseLoop;
	ACharacter* PoseLoopCharacter;
	float PoseLoopInterval;
	float PoseLoopLastTime;
	float PoseLoopAccTime;
	
};
