// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class AEnemyAfterParticle;
class UProjectileMovementComponent;
class UBoxComponent;
UCLASS()
class PORTFOLIO_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyProjectile();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	// 필요 컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyAfterParticle> AfterParticleClass;
public:
	UFUNCTION()
	UProjectileMovementComponent* GetProjectileMovement() {return ProjectileMovement;};
	void SetAttackPower(float _Power) { AttackPower = _Power; };
	
	// 필요 변수
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 400.f;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	float LastedTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	float AttackPower;

public:
	UFUNCTION()
	//virtual void OnProjectileHit(UPrimitiveComponent* _HitComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComponent, FVector _NormalImpulse, const FHitResult& Hit);
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void DestroyProjectile();

};
