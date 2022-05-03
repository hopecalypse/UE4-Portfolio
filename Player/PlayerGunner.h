// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PlayableCharacter.h"
#include "PlayerGunner.generated.h"

class AGunner_Grenade;
class UWidgetComponent;
class AMonsterGeneralCharacter;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API APlayerGunner : public APlayableCharacter
{
	GENERATED_BODY()

public:
	APlayerGunner();
private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// 추가 필요 컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USceneComponent* GunPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USceneComponent* RighthandPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UBoxComponent* EnemyTargetBox;

	// 타겟 관련
	AMonsterGeneralCharacter* TargetingEnemy;
	AMonsterGeneralCharacter* SavedTargetForMissile;
	TSubclassOf<AActor> TargetActorBP;
	AActor* TargetUIActor;
	UWidgetComponent* TargetWidgetComponent;
	


	// 기본 공격 함수 오버라이드
private:
	virtual void GetActionInput_AttackBasic() override;
	virtual void ActionTrigger_AttackBasic() override;
	AProjectileGeneral* SpawnProjectileToGun(TSubclassOf<AProjectileGeneral> _ProjectileClass, float _LifeTime, float _AttackDamage);

	// 스킬1 오버라이드
private:
	virtual void ActionTrigger_Skill1() override;

	// 스킬2 오버라이드
private:
	virtual void GetActionInput_Skill2() override;
	virtual void ActionTrigger_Skill2() override;
	int LeftMissileCount;
	UFUNCTION()
	void SpawnLeftMissiles_Skill2();

	// 스킬3 오버라이드
private:
	virtual void GetActionInput_Skill3() override;
	virtual void ActionTrigger_Skill3() override;
	virtual void MiscTrigger1() override;
	void ExecuteSkill3();
	void EndSkill3();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UAnimMontage* Skill3Montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<AActor> Skill3LaserClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<AActor> Skill3LaserHitClass;
	AActor* Skill3Laser;
	bool bOnSkill3;
	float Skill3AccTime;
	float Skill3LengthTime = 5.f;
	float Skill3Interval = 0.1f;
	float Skill3ExecuteAccTime;

private:
	virtual void EndActingFromNotify() override;

	// 공격 대상 설정 관련
private:
	TArray<AMonsterGeneralCharacter*> TargetEnemyList;
	UFUNCTION()
	void BoxOnOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void BoxEndOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// 가장 가까운 적 찾기
	AMonsterGeneralCharacter* GetNearestEnemy();
	void UpdateTargetingEnemy();
	
};
