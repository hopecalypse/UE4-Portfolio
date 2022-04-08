// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGeneral.h"

#include "AfterParticle.h"
#include "PortFolio.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectileGeneral::AProjectileGeneral()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LastedTime = 0.f;
	LifeTime = 3.f;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectole Movement Component"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Componenet"));
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle Componenet"));
	
	// 박스를 루트로 지정
	SetRootComponent(GetBoxComponent());
	GetParticleComponent()->SetupAttachment(GetBoxComponent());
	// 프로젝트일 Movement 컴포넌트의 대상을 박스로 지정
	GetProjectileMovement()->SetUpdatedComponent(GetBoxComponent());
	GetBoxComponent()->SetCollisionProfileName(TEXT("PlayerProjectile"));

	// Movement 프로퍼티 설정
	GetProjectileMovement()->ProjectileGravityScale = 0.f;
	GetProjectileMovement()->bInitialVelocityInLocalSpace = false;

	// 기본 속도
	GetProjectileMovement()->InitialSpeed = 800.f;
	GetProjectileMovement()->MaxSpeed = 1200.f;

	// 충돌 범위
}

// Called when the game starts or when spawned
void AProjectileGeneral::BeginPlay()
{
	Super::BeginPlay();
	
	GetBoxComponent()->OnComponentBeginOverlap.AddDynamic(this, &AProjectileGeneral::OnProjectileOverlap);
	GetBoxComponent()->OnComponentHit.AddDynamic(this, &AProjectileGeneral::OnProjectileHit);
}

// Called every frame
void AProjectileGeneral::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bDestroyOnHomingNull)
	{
		if(GetProjectileMovement()->HomingTargetComponent == nullptr)
			K2_DestroyActor();
	}

	if(!bDontDestroyAuto)
	{
		LastedTime += DeltaTime;
        if(LastedTime >= LifeTime)
        {
        	//LOGTEXT_LOG(TEXT("시간에 의해 제거"));
        	DestroyParticle();
        }
	}
}

void AProjectileGeneral::SetHitRange(float _HitRange)
{
	GetBoxComponent()->SetBoxExtent(FVector(_HitRange, _HitRange, _HitRange));
}

void AProjectileGeneral::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(bDontOverlap == true)	return;
	if(bAlreadyHit == true)		return;

	// 적과 Overlap 했을 때만
	AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(OtherActor);
	if(_HitMonster == nullptr)
		return;
	
	bAlreadyHit = true;

	// 몬스터에게 데미지 주고 파괴하기
	if(bApplyAttackDamage)
		AttackMonsterOnce(_HitMonster);

	// 다음 파티클 생성
	if(AfterDestroyParticle != nullptr)
	{
		FActorSpawnParameters _SpawnParams = {};
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_SpawnParams.bDeferConstruction = true;

		AAfterParticle* _AfterParticle = GetWorld()->SpawnActor<AAfterParticle>(AfterDestroyParticle, GetActorLocation(), GetActorRotation(), _SpawnParams);
		_AfterParticle->FinishSpawning(_AfterParticle->GetTransform());
		_AfterParticle->SetLifeSpan(2.f);
	}

	// 제거할지 결정
	if(bDestroyOnHit)
	{
		DestroyParticle();
	}
}

void AProjectileGeneral::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bHitOnTerrain)
		return;
		
	// Hit(블록) 시
	// 충돌 Sweep 체크하기
	FCollisionQueryParams _CollisionParams(NAME_None);
	TArray<AActor*> _ActorsToIgnore;
	TArray<FHitResult> _HitResults;
	bool _bHit = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 600.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::ForDuration, _HitResults, true);
	if(_bHit)
	{
		for(int i = 0; i < _HitResults.Num(); i++)
		{
			AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
			_HitMonster->ChangeMonsterHp(AttackDamage);
			_HitMonster->TrySetHitReaction();
		}
	}
	
	// 다음 파티클 생성
	if(AfterDestroyParticle != nullptr)
	{
		FActorSpawnParameters _SpawnParams = {};
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_SpawnParams.bDeferConstruction = true;

		AAfterParticle* _AfterParticle = GetWorld()->SpawnActor<AAfterParticle>(AfterDestroyParticle, GetActorLocation(), GetActorRotation(), _SpawnParams);
		_AfterParticle->FinishSpawning(_AfterParticle->GetTransform());
		_AfterParticle->SetLifeSpan(2.f);
	}
	
	// 제거하기
	DestroyParticle();
}

void AProjectileGeneral::AttackMonsterOnce(AMonsterGeneralCharacter* _HitMonster)
{
	// 몬스터에게 데미지 주기 + Hit Reaction 설정
	if(bApplyAttackDamage)
	{
		_HitMonster->ChangeMonsterHp(AttackDamage);
		_HitMonster->TrySetHitReaction();
	}
}

void AProjectileGeneral::DestroyParticle()
{
	if(bDestroying == true)			return;
	
	bDestroying = true;
	//ParticleComponent->DeactivateImmediate();
	//ParticleComponent->DestroyComponent();
	this->Destroy();
	//K2_DestroyActor();
}

