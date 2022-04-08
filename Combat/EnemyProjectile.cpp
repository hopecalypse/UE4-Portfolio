// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EnemyProjectile.h"

#include "EnemyAfterParticle.h"
#include "PortFolio.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayableCharacter.h"

AEnemyProjectile::AEnemyProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 생성
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	// 컴포넌트 설정
	SetRootComponent(BoxCollision);
	ParticleSystem->SetupAttachment(BoxCollision);
	ProjectileMovement->SetUpdatedComponent(BoxCollision);
	BoxCollision->SetCollisionProfileName(TEXT("EnemyProjectile"));
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bInitialVelocityInLocalSpace = false;

	ProjectileMovement->InitialSpeed = MoveSpeed;
	ProjectileMovement->MaxSpeed = MoveSpeed;

	LifeTime = 3.f;
	LastedTime = 0.f;
}

void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->InitialSpeed = MoveSpeed;
	ProjectileMovement->MaxSpeed = MoveSpeed;

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileBeginOverlap);
}

void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastedTime += DeltaTime;
	if(LastedTime > LifeTime)
		DestroyProjectile();
}

// void AEnemyProjectile::OnProjectileHit(UPrimitiveComponent* _HitComponent, AActor* _OtherActor,
// 	UPrimitiveComponent* _OtherComponent, FVector _NormalImpulse, const FHitResult& Hit)
// {
// 	LOGTEXT_ERROR(TEXT("Enemy 발사체 충돌"));
// 	// 플레이어인지 Check
// 	if(Cast<APlayableCharacter>(_OtherActor) != nullptr)
// 	{
// 		LOGTEXT_ERROR(TEXT("Enemy 발사체 충돌 Player"));
// 	}
// }

void AEnemyProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOGTEXT_ERROR(TEXT("Enemy 발사체 충돌"));
	if(Cast<APlayableCharacter>(OtherActor) != nullptr)
	{
		//LOGTEXT_ERROR(TEXT("Enemy 발사체 충돌 Player"));

		APlayableCharacter* _PlayerCharacter = Cast<APlayableCharacter>(OtherActor);

		// 무적상태이면 호출x
		if(_PlayerCharacter->GetIsInvincible())			return;

		// 플레이어 공격
		_PlayerCharacter->GetAttacked(AttackPower);

		// 만약 AfterEffect 있으면 생성
		if(AfterParticleClass != nullptr)
		{
			FActorSpawnParameters _SpawnParams = {};
			_SpawnParams.OverrideLevel = GetLevel();
			_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			_SpawnParams.bDeferConstruction = true;

			AEnemyAfterParticle* _AfterParticle = GetWorld()->SpawnActor<AEnemyAfterParticle>(AfterParticleClass, GetActorLocation(), GetActorRotation(), _SpawnParams);
			_AfterParticle->FinishSpawning(_AfterParticle->GetTransform());
		}

		// 파괴하기
		DestroyProjectile();
	}
}

void AEnemyProjectile::DestroyProjectile()
{
	K2_DestroyActor();
}
