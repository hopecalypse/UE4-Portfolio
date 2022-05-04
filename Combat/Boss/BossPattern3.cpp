// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Boss/BossPattern3.h"

#include "PortFolio.h"
#include "Core/AudioDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/SoundManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayableCharacter.h"

ABossPattern3::ABossPattern3()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene"));
	ExplodeParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explode Particle"));
	PreParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Pre Particle"));

	SetRootComponent(RootScene);
	ExplodeParticle->SetupAttachment(RootScene);
	PreParticle->SetupAttachment(RootScene);

	ExplodeParticle->SetAutoActivate(false);
}

void ABossPattern3::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), USoundManager::Instance()->Data->Boss_Skill3Ball, GetActorLocation(), FRotator::ZeroRotator,
		1.f, 1.f, 0.f, USoundManager::Instance()->Data->Attenuation);
}

void ABossPattern3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AccTime += DeltaTime;
	if(AccTime > 1.5f && !bExecuted)
	{
		bExecuted = true;
		PreParticle->Deactivate();
		ExplodeParticle->Activate();
		SetLifeSpan(2.f);

		TArray<AActor*> _IgnoreActor;
		TArray<FHitResult> _HitResults;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(),
			400.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6), false, _IgnoreActor, EDrawDebugTrace::ForDuration, _HitResults, true, FLinearColor::Red);

		for (FHitResult _HitResult : _HitResults)
		{
			APlayableCharacter* _Player = Cast<APlayableCharacter>(_HitResult.Actor);
			if(_Player != nullptr)
			{
				LOGTEXT_LOG(TEXT("보스 패턴3 플레이어 Hit"));
				_Player->GetAttacked(30.f);
			}
		}

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), USoundManager::Instance()->Data->Boss_Skill3Hit, GetActorLocation(), FRotator::ZeroRotator,
		1.f, 1.f, 0.f, USoundManager::Instance()->Data->Attenuation);
	}
}

