// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectCharacter/SelectCharacter.h"

#include "PortFolio.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/CharacterSelectGameMode.h"
#include "Kismet/GameplayStatics.h"

ASelectCharacter::ASelectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bFindCameraComponentWhenViewTarget = true;

	SelectCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Select Camera"));
	SelectCamera->SetupAttachment(RootComponent);
}

void ASelectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 게임모드 찾기
	SelectGameMode = Cast<ACharacterSelectGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// 선택 함수 델리게이트 등록
	GetCapsuleComponent()->OnBeginCursorOver.AddDynamic(this, &ASelectCharacter::HoverSelectCharacter);
	GetCapsuleComponent()->OnEndCursorOver.AddDynamic(this, &ASelectCharacter::UnHoverSelectCharacter);
	GetCapsuleComponent()->OnClicked.AddUniqueDynamic(this, &ASelectCharacter::ClickSelectCharacter);
}

void ASelectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASelectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASelectCharacter::HoverSelectCharacter(UPrimitiveComponent* _OnEndCursorOver)
{
	//LOGAUTO_LOG;
}

void ASelectCharacter::UnHoverSelectCharacter(UPrimitiveComponent* _OnBeginCursorOver)
{
	//LOGAUTO_LOG;
}

void ASelectCharacter::ClickSelectCharacter(UPrimitiveComponent* _TouchedComponent, FKey _ButtonPressed)
{
	//LOGAUTO_WARN;
	//bIsSelecting = true;
	
	// 게임모드 선택 캐릭터 함수 호출 -> 애니메이션 재생하기
	//SelectGameMode->Select_FromCharacter(this);
	//Cast<ACharacterSelectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Select_FromCharacter(this);
	//LOGTEXT_LOG(TEXT("%s"), *UGameplayStatics::GetGameMode(GetWorld())->GetName())
	//LOGTEXT_LOG(TEXT("%s"), *SelectGameMode->GetName());
	SelectGameMode->Select_FromCharacter(this);
}

