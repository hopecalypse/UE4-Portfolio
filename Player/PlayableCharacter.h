// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/CustomStructs.h"
#include "GameFramework/Character.h"
#include "PlayableCharacter.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class UDungeonCell;
class UDungeonRoom;
class AProjectileGeneral;
class UPlayerAnimInstance;
class USpringArmComponent;
class UCameraComponent;
class UPlayerHUD;

UCLASS()
class PORTFOLIO_API APlayableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayableCharacter();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// ! 필요 컴포넌트
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Created Components", meta=(AllowPrivateAccess))
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Created Components", meta=(AllowPrivateAccess))
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Created Components", meta=(AllowPrivateAccess))
	UPlayerAnimInstance* AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Created Components", meta=(AllowPrivateAccess))
	USceneCaptureComponent2D* FaceCaptureCamera;
	

	// ! 조작 관련
public:
	UPROPERTY()
	float CurrentMoveForwardValue;
	UPROPERTY()
	float CurrentMoveRightValue;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void RotateYaw(float AxisValue);
	void RotatePitch(float AxisValue);
	UFUNCTION()
	void StartJump();
	// 캐릭터 교체 관련
	void SwitchCharacter();
	// Action 함수 정의
	UFUNCTION()
	virtual void GetActionInput_AttackBasic();
	UFUNCTION()
	virtual void GetActionInput_Dodge();
	UFUNCTION()
	virtual void GetActionInput_Skill1();
	UFUNCTION()
	virtual void GetActionInput_Skill2();
	UFUNCTION()
	virtual void GetActionInput_Skill3();
	// Action 트리거
	virtual void ActionTrigger_AttackBasic();
	virtual void ActionTrigger_Dodge();
	virtual void ActionTrigger_Skill1();
	virtual void ActionTrigger_Skill2();
	virtual void ActionTrigger_Skill3();
	// 쿨다운 함수들 정의
	void Cooldown_AttackBasic();
	void Cooldown_Skill1();
	void Cooldown_Skill2();
	void Cooldown_Skill3();
	UFUNCTION()
	void OnCooldownEnd_AttackBasic();
	UFUNCTION()
	void OnCooldownEnd_Skill1();
	UFUNCTION()
	void OnCooldownEnd_Skill2();
	UFUNCTION()
    void OnCooldownEnd_Skill3();
	// Casting 관련
	bool bCastingIgnoreRot;
	UFUNCTION()
	virtual void MiscTrigger1();
	UFUNCTION()
	virtual void MiscTrigger2();


	// ! 공격 실행 함수 //
protected:
	virtual void MeleeSweepAttack(float _AttackRange, float _AttackPower, float _AttackRadius = 120.f);
	virtual void ShootMovingProjectile(TSubclassOf<AProjectileGeneral> _TargetProjectile, float _LifeTime = 2.f, bool _bAttackDMG = false, float _AttackDamage = 10.f, float _StartPosition = 50.f);

	// ! 공격 받기 함수 //
public:
	void GetAttacked(float _AttackDamage);
	void StartDeath();

public:
	UFUNCTION()
	void EndJumpFromNotify();
	UFUNCTION()
	void EndActingFromNotify();

public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentMoveForward() const { return CurrentMoveForwardValue; };
	UFUNCTION(BlueprintCallable)
	float GetCurrentMoveRight() const { return CurrentMoveRightValue; };


	// ! 인게임 필요 자료형 //
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerClass Class;
	
	// 현재 캐릭터 정보(스탯)
	UPROPERTY(VisibleAnywhere, Category="Player Info")
	FPlayerInfo PlayerInfo;

	// 플레이어 스탯 가져오기 + 설정
	void SetPlayerStat(int16 Level);

	// 공격, 스킬 정보
	UPROPERTY(EditAnywhere, Category="Acting Info")
	FClassActingInfo ActingInfos;

	// Acting 쿨타임 관련
	bool bBasicAttackCooling;
	bool bSkill1Cooling;
	bool bSkill2Cooling;
	bool bSkill3Cooling;

private:
	// 플레이어 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Player Status")
	EPlayerState CurrentState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Player Status")
	EActingType CurrentActingType;
	// 현재 무적 여부
	UPROPERTY(VisibleAnywhere, Category="Player Status")
	bool bOnInvincible;
	
public:
	// 현재 State
	UFUNCTION(BlueprintCallable)
	EPlayerState GetCurrentState() const { return CurrentState; };
	UFUNCTION()
	void SetCurrentState(EPlayerState NextState);
	UFUNCTION(BlueprintCallable)
	// 현재 Acting Type
	EActingType GetCurrentActingType() const { return CurrentActingType; };
	UFUNCTION()
	void SetCurrentActingType(EActingType NextAct);
	
	// 무적Invincible 상태
	UFUNCTION()
	bool GetIsInvincible() const { return bOnInvincible; };
	UFUNCTION()
	void SetIsInvincible(bool Value) { bOnInvincible = Value; };
	

private:
	UFUNCTION()
	void TickUpdateStatus();

public:
	//void ChangeHp(float _AtkPoint);
	void ChangeMp(float _ManaPoint);
	
	// ! UI 관련	//
private:
	UPlayerHUD* PlayerHUD;
public:
	void SetPlayerHUD(UPlayerHUD* _PlayerHUD) { PlayerHUD = _PlayerHUD; };

	// 던전 관련
private:
	void UpdateDungeonLocation();

	// 미니맵
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* MinimapArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* MinimapSceneCapture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* MinimapRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UPaperSpriteComponent* MinimapSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UPaperSprite* PlayerSprite;
	
};
