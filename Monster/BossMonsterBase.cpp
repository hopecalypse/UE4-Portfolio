// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BossMonsterBase.h"

#include "Components/CapsuleComponent.h"

ABossMonsterBase::ABossMonsterBase()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(176.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
}
