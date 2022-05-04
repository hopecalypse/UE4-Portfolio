// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "BossMonsterBase.generated.h"

class ABossPattern3;
class ABossPattern1;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API ABossMonsterBase : public AMonsterGeneralCharacter
{
	GENERATED_BODY()

public:
	ABossMonsterBase();

public:
	EMonsterAttackType AttackType;
	void SetIsAttacking(bool _Value);
	bool GetIsAttacking() { return bIsAttacking; }

	FVector SpawnLocation;
private:
	bool bIsAttacking;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABossPattern1> Pattern1Actor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Pattern2Montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Pattern3Montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABossPattern3> Pattern3Actor;

private:
	virtual void EndAttack_FromNotify() override;
	virtual void AttackTrigger_FromNotify() override;

private:
	virtual void TrySetHitReaction() override;
	virtual void ChangeMonsterHp(float _HpChangeValue) override;
	virtual void EndDying_FromNotify() override;

public:
	void PlayBossSound_FromNotify();
	
	
};
