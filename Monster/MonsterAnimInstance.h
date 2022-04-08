// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

enum class EMonsterState : uint8;
class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


private:
	// 애님인스턴스 시스템 함수
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 필수 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Monster Anim Properties", meta=(AllowPrivateAccess = "true"))
	AMonsterGeneralCharacter* MonsterCharacter;

	// State, ActingType 가져오기
	UFUNCTION(BlueprintCallable)
	EMonsterState GetMonsterState() const;

	////////////////////////////////////////////////////////
	// 애님인스턴스 Notifty 함수
	UFUNCTION()
	void AnimNotify_TriggerAttack();
	UFUNCTION()
	void AnimNotify_EndAttack();
	UFUNCTION()
	void AnimNotify_EndHitReact();
	UFUNCTION()
	void AnimNotify_EndDying();
};
