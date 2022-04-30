// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "BossMonsterBase.generated.h"

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
	bool bIsAttacking;

private:
	virtual void EndAttack_FromNotify() override;

	
	
};
