// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Gunner/Gunner_Grenade.h"

#include "PortFolio.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AGunner_Grenade::AGunner_Grenade()
{
	// Static Mesh 추가
	GrenadeStaticmesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade StaticMesh"));
	GrenadeStaticmesh->SetCollisionProfileName(TEXT("NoCollision"));
	GrenadeStaticmesh->SetGenerateOverlapEvents(false);
	GrenadeStaticmesh->SetupAttachment(RootComponent);
	GrenadeStaticmesh->SetCanEverAffectNavigation(false);
	
	// 제거되지 않게 설정
	SetLifeTime(100.f);
	GetBoxComponent()->SetBoxExtent(FVector(10.f, 10.f, 10.f));
	
	// Projectile Movement 설정
	GetProjectileMovement()->InitialSpeed = 1600.f;
	GetProjectileMovement()->MaxSpeed = 2000.f;
	GetProjectileMovement()->bShouldBounce = true;
	GetProjectileMovement()->ProjectileGravityScale = 1.f;
	GetProjectileMovement()->Friction = 0.6f;
	GetProjectileMovement()->Bounciness = 0.5f;

	// Particle 설정
	ParticleComponent->SetAutoActivate(false);

	// 초기 설정
	bExploding = false;
}

void AGunner_Grenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(FMath::IsNearlyZero(GetVelocity().Size()))
		ExplodeGrenade();
}

void AGunner_Grenade::InitGrenade(FVector _ForwardVector, float _Damage)
{
	Damage = _Damage;
	GetProjectileMovement()->Velocity = _ForwardVector;
}

void AGunner_Grenade::ExplodeGrenade()
{
	if(bExploding)	return;
	bExploding = true;

	GrenadeStaticmesh->SetVisibility(false);
	ParticleComponent->Activate();

	// Sphere Collision 검사
	float _Radius = 600.f;
	TArray<AActor*> _Ignores;
	TArray<FHitResult> _HitResultArray;
	FCollisionQueryParams _Params(NAME_None, false, this);
	bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), _Radius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), false, _Ignores, EDrawDebugTrace::Type::None, _HitResultArray, true);

	if(_bResult)
	{
		for(int i = 0; i < _HitResultArray.Num(); i++)
		{
			// 몬스터로 변환
			AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResultArray[i].Actor);
			if(_HitMonster != nullptr)
			{
				// 데미지 주기
				_HitMonster->ChangeMonsterHp(Damage);
				_HitMonster->TrySetHitReaction();
				// 밀려나게 하기
				// FVector _LaunchVelocity = GetActorLocation() - _HitMonster->GetActorLocation();
				// _HitMonster->LaunchCharacter(_LaunchVelocity.GetSafeNormal() * 100.f, true, true);
			}
		}
	}

	SetLifeSpan(1.f);
}
