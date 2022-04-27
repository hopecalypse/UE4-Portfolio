// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"

#include "Components/SphereComponent.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Root Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	
	SetRootComponent(RootSphere);
	StaticMesh->SetupAttachment(RootSphere);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

