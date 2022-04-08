// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/AfterParticle.h"

#include "Particles/ParticleSystemComponent.h"

// Sets default values
AAfterParticle::AAfterParticle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System Component"));
	SetRootComponent(ParticleSystemComponent);
}

// Called when the game starts or when spawned
void AAfterParticle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAfterParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAfterParticle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

