// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

enum class EActingType : uint8;
enum class EPlayerState : uint8;
class APlayableCharacter;
// 선언
enum class EPlayerStatus : uint8;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	// 생성자
	UPlayerAnimInstance();

private:
	// 애니메이션 업데이트 시스템 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	
	// 필수 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	APlayableCharacter* PlayerCharacter;

	// 애니메이션 필요 변수
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	float MoveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	float MoveForwardValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	float MoveRightValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	bool bOnJumping;

	// 플레이어 State, ActingType 가져오기
public:
	UFUNCTION(BlueprintCallable)
	EActingType GetPlayerActingType() const { if(PlayerCharacter != nullptr){ return PlayerCharacter->GetCurrentActingType(); } else{ return EActingType::E_None; }}
	UFUNCTION(BlueprintCallable)
	EPlayerState GetCurrentPlayerState() const { if(PlayerCharacter != nullptr){ return PlayerCharacter->GetCurrentState(); }else{ return EPlayerState::E_Idle; }}

	//애니메이션 재생 함수
public:
	UFUNCTION()
	void PlayActingAnimation(EActingType NextActingType);

	//애니메이션 재생 필요 변수
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Anim Properties", meta=(AllowPrivateAccess=true))
	int BasicAttackNum;


	// 노티파이 함수
public:
	UFUNCTION()
	void AnimNotify_EndJump();
	UFUNCTION()
	void AnimNotify_EndAttackBasic();
	UFUNCTION()
	void AnimNotify_EndDodge();
	UFUNCTION()
	void AnimNotify_EndSkill1();
	UFUNCTION()
	void AnimNotify_EndSkill2();
	UFUNCTION()
	void AnimNotify_EndSkill3();

	UFUNCTION()
	void AnimNotify_TriggerAttackBasic();
	UFUNCTION()
	void AnimNotify_TriggerDodge();
	UFUNCTION()
	void AnimNotify_TriggerSkill1();
	UFUNCTION()
	void AnimNotify_TriggerSkill2();
	UFUNCTION()
	void AnimNotify_TriggerSkill3();

	UFUNCTION()
	void AnimNotify_Misc1();
	UFUNCTION()
	void AnimNotify_Misc2();
	
};
