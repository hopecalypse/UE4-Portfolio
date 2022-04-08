// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/ProjectileGeneral.h"
#include "Gunner_Missile.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AGunner_Missile : public AProjectileGeneral
{
	GENERATED_BODY()


public:
	AGunner_Missile();

private:
	virtual void Tick(float DeltaSeconds) override;
	
public:
	void InitMissile(USceneComponent* _HomingTargetComponent, float _Damage);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UParticleSystemComponent* ExplodeParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UParticleSystemComponent* TrailParticle;

private:
	bool bInited;
	USceneComponent* HomingTarget;

	float Damage;

private:
	virtual void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	void DestroyMissile();
};
