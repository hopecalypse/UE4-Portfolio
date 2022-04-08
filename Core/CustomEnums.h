// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomEnums.generated.h"

/**
 * 
 */
class PORTFOLIO_API FCustomEnums
{
public:
	FCustomEnums();
	~FCustomEnums();
};

// 정의 ~~~

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	E_Warrior,
	E_Mage,
	E_Gunner,
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	E_Idle,
	E_Moving,
	E_Jumping,
	E_ActingStatic,
	E_ActingMoving,
	E_ETC,
	E_Dying
};

UENUM(BlueprintType)
enum class EActingType : uint8
{
	E_None,
	E_Dodge,
	E_AttackBasic,
	E_AttackSkill1,
	E_AttackSkill2,
	E_AttackSkill3,
	E_AttackSkill4,

};



///////////////////////////////////////////////////////////////////////////////////
// 몬스터 관련
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	E_Spawning,
	E_Idle,
	E_MovingIdle,
	E_MovingTrace,
	E_Attack,
	E_HitReact,
	E_Dying,
};