// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Boss/BossPattern1.h"

#include "Components/SphereComponent.h"
#include "Core/AudioDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SoundManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayableCharacter.h"

ABossPattern1::ABossPattern1()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Root Collision"));
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle Orb"));
	HitParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Hit Particle"));

	SetRootComponent(RootCollision);
	Particle->SetupAttachment(RootCollision);
	HitParticle->SetupAttachment(RootCollision);

	Particle->SetRelativeLocation(FVector(0.f, -120.f, -80.f));
	Particle->SetRelativeScale3D(FVector(2.f));
	
	HitParticle->SetAutoActivate(false);

	RootCollision->SetCollisionProfileName(TEXT("OverlapPlayer"));
}

void ABossPattern1::BeginPlay()
{
	Super::BeginPlay();

	RootCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossPattern1::OnOverlapPlayer);
	SetLifeSpan(8.f);
}

void ABossPattern1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 1.f));
}

void ABossPattern1::OnOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayableCharacter* _Player = Cast<APlayableCharacter>(OtherActor);
	if(_Player != nullptr)
	{
		_Player->GetAttacked(20.f);
		Particle->Deactivate();
		HitParticle->Activate();

		SetLifeSpan(2.f);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), USoundManager::Instance()->Data->Boss_Skill1Hit, GetActorLocation(), FRotator::ZeroRotator,
			1.f, 1.f, 0.f, USoundManager::Instance()->Data->Attenuation);
	}
}

