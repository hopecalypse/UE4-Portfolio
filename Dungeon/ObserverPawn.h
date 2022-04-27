// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ObserverPawn.generated.h"

class UCameraComponent;
UCLASS()
class PORTFOLIO_API AObserverPawn : public APawn
{
	GENERATED_BODY()

public:
	AObserverPawn();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

public:
	UCameraComponent* Camera;
	bool bFreeze;

	// 입력 관련
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	// 이동
	FVector MoveVector;
	void MoveForward(float _Value);
	void MoveRight(float _Value);
	void Move(float _DeltaTime);
	// 회전
	FRotator Rotator;
	void RotateYaw(float _Value);
	void RotatePitch(float _Value);
	void Rotate(float _DeltaTime);

	void ResetTransform();

};
