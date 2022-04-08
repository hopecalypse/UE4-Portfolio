// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/CustomEnums.h"
#include "GameFramework/Character.h"
#include "SelectCharacter.generated.h"

class UCameraComponent;
class ACharacterSelectGameMode;
UCLASS()
class PORTFOLIO_API ASelectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASelectCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UCameraComponent* SelectCamera;

	// 필요 변수
private:
	ACharacterSelectGameMode* SelectGameMode;
	UAnimInstance* SelectAnimInstance;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	EPlayerClass PlayerClass;

	// 애니메이션 관련 변수
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSelecting;

	// 캐릭터 선택 함수
public:
	UFUNCTION()
	void HoverSelectCharacter(UPrimitiveComponent* _OnBeginCursorOver);
	UFUNCTION()
	void UnHoverSelectCharacter(UPrimitiveComponent* _OnEndCursorOver);
	UFUNCTION()
	void ClickSelectCharacter(UPrimitiveComponent* _TouchedComponent, FKey _ButtonPressed);
	EPlayerClass GetSelectClass() { return PlayerClass; }

	// 캐릭터 애니메이션 관련 함수
	

};
