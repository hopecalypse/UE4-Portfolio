// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Gunner/Gunner_Missile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AGunner_Missile::AGunner_Missile()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile Static Mesh"));
	ExplodeParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explode Particle"));
	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle"));

	StaticMesh->SetupAttachment(RootComponent);
	ExplodeParticle->SetupAttachment(RootComponent);
	TrailParticle->SetupAttachment(RootComponent);

	// 컴포넌트 설정
	GetBoxComponent()->SetBoxExtent(FVector(10.f, 10.f, 10.f));
	ExplodeParticle->SetAutoActivate(false);

	// Projectile 설정
	GetProjectileMovement()->bRotationFollowsVelocity = true;
	GetProjectileMovement()->InitialSpeed = 200.f;
	GetProjectileMovement()->MaxSpeed = 1000.f;
	GetProjectileMovement()->bIsHomingProjectile = true;
	GetProjectileMovement()->HomingAccelerationMagnitude = 5000.f;
	
}

void AGunner_Missile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bInited)
	{
		if(HomingTarget == nullptr)
			DestroyMissile();
	}
}

void AGunner_Missile::InitMissile(USceneComponent* _HomingTargetComponent, float _Damage)
{
	Damage = _Damage;
	HomingTarget = _HomingTargetComponent;
	// 1. 현재 위치에서 위쪽 랜덤 위치로 발사하기
	FVector _RandomPosition = FVector(FMath::RandRange(-1000.f, 1000.f), -200.f, 100.f);
	GetProjectileMovement()->Velocity = _RandomPosition.GetSafeNormal() * 1000.f; 
	GetProjectileMovement()->HomingTargetComponent = HomingTarget;

	bInited = true;
}

void AGunner_Missile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	GetBoxComponent()->SetGenerateOverlapEvents(false);
	
	AMonsterGeneralCharacter* _Monster = Cast<AMonsterGeneralCharacter>(OtherActor);
	if(_Monster != nullptr)
	{
		_Monster->TrySetHitReaction();
		_Monster->ChangeMonsterHp(Damage);
	}
	
	DestroyMissile();
}

void AGunner_Missile::DestroyMissile()
{
	ExplodeParticle->Activate();
	StaticMesh->SetVisibility(false);
	GetProjectileMovement()->StopMovementImmediately();
	SetLifeSpan(0.5f);
}
