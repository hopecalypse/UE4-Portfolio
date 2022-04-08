// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoseEffectActor.generated.h"

class UPoseableMeshComponent;
UCLASS()
class PORTFOLIO_API APoseEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APoseEffectActor();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	// 초기화하기
public:
	void InitPoseEffect(ACharacter* _CopyCharacter, float _LastTime);
	
	// 포즈 효과 관련
private:
	UPoseableMeshComponent* PoseableMesh;
	UMaterial* PoseMaterial;
	float AccLifeTime;
	

};
