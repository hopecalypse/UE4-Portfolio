// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomEnums.h"
#include "Combat/EnemyProjectile.h"
#include "Engine/DataTable.h"
#include "CustomStructs.generated.h"

class AProjectileGeneral;
/**
 * 
 */
class PORTFOLIO_API FCustomStructs
{
public:
	FCustomStructs();
	~FCustomStructs();
};

// 정의 ~~~

USTRUCT(Atomic, BlueprintType)
struct FPlayerInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FName PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	EPlayerClass Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float MaxHp = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float CurrentHp = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    float MaxMp = 50.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    float CurrentMp = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float CurrentExp = 0.f;
};

// 플레이어 레벨 스탯 테이블
USTRUCT(Atomic, BlueprintType)
struct FClassStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasicAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Skill1Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Skill2Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Skill3Damage;
};

// 클래스의 스킬 정보 정의 구조체
USTRUCT(Atomic, BlueprintType)
struct FClassActingInfo
{
	GENERATED_USTRUCT_BODY()

	// 기본 공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UTexture2D* BasicAttackImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool BasicAttackMovable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsBasicAttackMeleeSweep = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float BasicAttackMeleeRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float BasicAttackPower = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    float BasicAttackCooldown = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectileGeneral> BasicAttackProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bBaiscAttackDamageAtProjectile = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float BasicAttackProjectileLifeTime = 4.f;
	

	// 스킬 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UTexture2D* Skill1Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool Skill1AttackMovable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsSkill1MeleeSweep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill1MeleeRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill1AttackPower = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill1Cooldown = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectileGeneral> Skill1Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bSkill1DamageAtProjectile = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float Skill1ProjectileLifeTime = 4.f;
	

	// 스킬 2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UTexture2D* Skill2Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool Skill2AttackMovable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsSkill2MeleeSweep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill2MeleeRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill2AttackPower = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill2Cooldown = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectileGeneral> Skill2Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    bool bSkill2DamageAtProjectile = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float Skill2ProjectileLifeTime = 4.f;
	

	// 스킬 3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UTexture2D* Skill3Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool Skill3AttackMovable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsSkill3MeleeSweep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill3MeleeRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill3AttackPower = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Skill3Cooldown = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectileGeneral> Skill3Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bSkill3DamageAtProjectile = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float Skill3ProjectileLifeTime = 4.f;

	// 기타
	// Casting 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AActor> CastingDecalActorClass;
	
	
};


/////////////////////////////////////////////////////////////////////////////////////////////
// 몬스터 관련
USTRUCT(Atomic, BlueprintType)
struct FMonsterInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FName MonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float MaxHp = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float CurrentHp = 100.f;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float MoveIdleSpeed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float MoveTraceSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool bPatrolRandomPoints = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float PatrolRandomRadius = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float DetectRange = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float AttackPoint = 10.f;
	
};

USTRUCT(Atomic, BlueprintType)
struct FMonsterAttackInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsMeleeSweep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AEnemyProjectile> AttackProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float WaitAfter;
	
};
