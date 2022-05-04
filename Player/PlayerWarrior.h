// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "PlayerWarrior.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API APlayerWarrior : public APlayableCharacter
{
	GENERATED_BODY()

public:
	APlayerWarrior();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// 필요 오브젝트
	APlayerController* PlayerController;

	// 기본공격
private:
	virtual void GetActionInput_AttackBasic() override;
	virtual void ActionTrigger_AttackBasic() override;

	// 스킬1 함수 오버라이드
private:
	virtual void GetActionInput_Skill1() override;
	virtual void ActionTrigger_Skill1() override;

	// 스킬2 함수 오버라이드
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* Skill2Montage;
	FVector BeforeDashLocation;
	bool bDashRotating;
	int Skill2Count;
	virtual void GetActionInput_Skill2() override;
	virtual void ActionTrigger_Skill2() override;
	void TickRotatingSkill2(float _DeltaTime);

	// 스킬3 함수 오버라이드
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* Skill3Montage;
	bool bIsCasting;
	AActor* CastingDecalActor;
	bool bIsFlying;
	FVector SelectLocation;
	float SelectDistance;
	FVector FlyingLocation;
	virtual void GetActionInput_Skill3() override;
	virtual void ActionTrigger_Skill3() override;
	void TickCastingSkill3(float _DeltaTime);
	void Execute_Skill3();
	void TickFlySkill3(float _DeltaTime);
	virtual void MiscTrigger1() override;
	virtual void MiscTrigger2() override;

	
		
};
