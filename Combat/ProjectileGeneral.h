// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileGeneral.generated.h"

class AAfterParticle;
class AMonsterGeneralCharacter;
class UBoxComponent;
class UProjectileMovementComponent;
UCLASS()
class PORTFOLIO_API AProjectileGeneral : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileGeneral();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;
	
	// 필요 컴포넌트
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleComponent;
	
	//필요 변수
private:
	float LastedTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LifeTime;
	float AttackDamage;
	bool bApplyAttackDamage;
	bool bAlreadyHit;
	bool bDestroying;
public:
	bool bDontOverlap;
	bool bDontDestroyAuto;
	bool bDestroyOnHomingNull;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool bDestroyOnHit;
	bool bHitOnTerrain;
public:
	void SetLifeTime(float _LifeTime) { LifeTime = _LifeTime; };
	void SetAttackDamage(float _Damage) { AttackDamage = _Damage; bApplyAttackDamage = true; };
	void SetHitRange(float _HitRange);

	// 블루프린트 필요 변수
private:
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAfterParticle> AfterDestroyParticle;


	// 컴포넌트 정의 함수(상속받은 클래스에서 정의)
protected:
	void SetProjectileMovement(UProjectileMovementComponent* _Component) { ProjectileMovementComponent = _Component; };
	void SetBoxComponent(UBoxComponent* _BoxComponent) { BoxComponent = _BoxComponent; };
	void SetParticleComponent(UParticleSystemComponent* _ParticleComponent) { ParticleComponent = _ParticleComponent; };

public:
	UProjectileMovementComponent* GetProjectileMovement() { return ProjectileMovementComponent; };
	UBoxComponent* GetBoxComponent() { return BoxComponent; };
	UParticleSystemComponent* GetParticleComponent() { return ParticleComponent; };

	// Box의 충돌 델리게이트에 추가할 함수 정의
public:
	UFUNCTION()
	virtual void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void AttackMonsterOnce(AMonsterGeneralCharacter* _HitMonster);

	UFUNCTION()
	virtual void DestroyParticle();
	
	

};
