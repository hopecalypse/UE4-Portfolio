// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BossMonsterBase.h"

#include "PaperSpriteComponent.h"
#include "PortFolio.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

ABossMonsterBase::ABossMonsterBase()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(176.f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	GetMesh()->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));

	MinimapSprite->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	HpbarWidgetComponent->SetDrawSize(FVector2D(0.f, 0.f));
}

void ABossMonsterBase::EndAttack_FromNotify()
{
	Super::EndAttack_FromNotify();

	LOGTEXT_ERROR(TEXT("보스 AttackEnd From Notify"));
	bIsAttacking = false;
}
