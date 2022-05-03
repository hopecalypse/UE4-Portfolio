// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "PlayerMage.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API APlayerMage : public APlayableCharacter
{
	GENERATED_BODY()


public:
	APlayerMage();
private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	APlayerController* PlayerController;

	// 기본 공격 오버라이드
private:
	virtual void ActionTrigger_AttackBasic() override;
	TArray<AProjectileGeneral*> BasicProjectiles;

	// 스킬1 오버라이드
	virtual void ActionTrigger_Skill1() override;

	// 스킬2 오버라이드
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* Skill2Montage;
	virtual void GetActionInput_Skill2() override;
	virtual void ActionTrigger_Skill2() override;

	// 스킬3 오버라이드
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* Skill3Montage;
	bool bIsCasting;
	AActor* CastingDecalActor;
	virtual void GetActionInput_AttackBasic() override;
	virtual void GetActionInput_Skill3() override;
	virtual void ActionTrigger_Skill3() override;
	void TickCastingSkill3(float _DeltaTime);
	void ExecuteSkill3();
	virtual void MiscTrigger1() override;
	virtual void MiscTrigger2() override;

	virtual void EndActingFromNotify() override;
	
	
};
