// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EnemyAfterParticle.h"

#include "Particles/ParticleSystemComponent.h"

// Sets default values
AEnemyAfterParticle::AEnemyAfterParticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	SetRootComponent(ParticleSystemComponent);
	ParticleSystemComponent->bAutoDestroy = true;
}

// Called when the game starts or when spawned
void AEnemyAfterParticle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyAfterParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

