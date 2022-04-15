// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/ObserverPawn.h"

#include "Camera/CameraComponent.h"

AObserverPawn::AObserverPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
}

void AObserverPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObserverPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bFreeze)
	{
		Move(DeltaTime);
		Rotate(DeltaTime);
	}
}

void AObserverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AObserverPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AObserverPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("RotateYaw"), this, &AObserverPawn::RotateYaw);
	PlayerInputComponent->BindAxis(TEXT("RotatePitch"), this, &AObserverPawn::RotatePitch);
}

void AObserverPawn::MoveForward(float _Value)
{
	MoveVector += GetActorForwardVector() * _Value;
}

void AObserverPawn::MoveRight(float _Value)
{
	MoveVector += GetActorRightVector() * _Value;
}

void AObserverPawn::Move(float _DeltaTime)
{
	FVector _Vec = MoveVector * _DeltaTime * 8000.f;

	SetActorLocation(GetActorLocation() + _Vec);
	MoveVector = FVector(0.f, 0.f, 0.f);
}

void AObserverPawn::RotateYaw(float _Value)
{
	Rotator += FRotator(0.f, 1.f, 0.f) * _Value;
}

void AObserverPawn::RotatePitch(float _Value)
{
	Rotator += FRotator(-1.f, 0.f, 0.f) * _Value;
}

void AObserverPawn::Rotate(float _DeltaTime)
{
	FRotator _Rot = Rotator * _DeltaTime * 200.f;
	_Rot += GetActorRotation();
	_Rot.Roll = 0.f;

	SetActorRotation(_Rot);
	Rotator = FRotator::ZeroRotator;
}

