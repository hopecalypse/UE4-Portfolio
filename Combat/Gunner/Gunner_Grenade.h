// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/ProjectileGeneral.h"
#include "Gunner_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AGunner_Grenade : public AProjectileGeneral
{
	GENERATED_BODY()

public:
	AGunner_Grenade();

private:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* GrenadeStaticmesh;

public:
	void InitGrenade(FVector _ForwardVector, float _Damage);

private:
	float Damage;
	bool bExploding;
	void ExplodeGrenade();
	
};
