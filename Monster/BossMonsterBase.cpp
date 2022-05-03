// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BossMonsterBase.h"

#include "PaperSpriteComponent.h"
#include "PortFolio.h"
#include "Combat/Boss/BossPattern1.h"
#include "Combat/Boss/BossPattern3.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "Dungeon/DungeonManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayableCharacter.h"
#include "UI/PlayerHUD.h"

ABossMonsterBase::ABossMonsterBase()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(176.f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	GetMesh()->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));

	MinimapSprite->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	//HpbarWidgetComponent->SetDrawSize(FVector2D(0.f, 0.f));
	
}

void ABossMonsterBase::SetIsAttacking(bool _Value)
{
	//LOGTEXT_LOG(TEXT("IsAttacking 설정(%d)"), _Value);
	bIsAttacking = _Value;
}

void ABossMonsterBase::EndAttack_FromNotify()
{
	//Super::EndAttack_FromNotify();
	bIsAttacking = false;
	MonsterAnimInstance->Montage_Stop(0.1f);
	SetMonsterState(EMonsterState::E_Idle);

	LOGTEXT_ERROR(TEXT("보스 AttackEnd From Notify"));
}

void ABossMonsterBase::AttackTrigger_FromNotify()
{
	//LOGTEXT_LOG(TEXT("보스 Attack Trigger"));
	if(AttackType == EMonsterAttackType::E_BasicAttack)
	{
		LOGTEXT_LOG(TEXT("보스 기본공격 스킬 트리거"));
		
		TArray<AActor*> _IgnoreActor;
		TArray<FHitResult> _HitResults;
		FVector _Location = GetMesh()->GetSocketLocation(TEXT("HammerCenter")) - GetActorForwardVector() * 100.f;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), _Location, _Location - FVector(0.f, 0.f, 300.f),
			140.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6), false, _IgnoreActor, EDrawDebugTrace::ForDuration, _HitResults, true, FLinearColor::Red);

		for (FHitResult _HitResult : _HitResults)
		{
			APlayableCharacter* _Player = Cast<APlayableCharacter>(_HitResult.Actor);
			if(_Player != nullptr)
			{
				LOGTEXT_LOG(TEXT("보스 기본공격 플레이어 Hit"));
				_Player->GetAttacked(15.f);
			}
		}
	}
	if(AttackType == EMonsterAttackType::E_Pattern1)
	{
		LOGTEXT_LOG(TEXT("보스 패턴1 스킬 트리거"));
		// 부딫치면 데미지 입는 액터 흩뿌리기

		FActorSpawnParameters _SpawnParams;
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for(int i = 0; i < 3; i++)
		{
			FVector _HandLocation = GetMesh()->GetSocketLocation(TEXT("LeftHand"));
			ABossPattern1* _SpawnBall = GetWorld()->SpawnActor<ABossPattern1>(Pattern1Actor, _HandLocation, FRotator::ZeroRotator, _SpawnParams);

			float _RandForward = FMath::RandRange(0.f, 1200.f);
			float _RandRight = FMath::RandRange(-800.f, 800.f);
			FVector _RandLocation =  GetActorForwardVector() * _RandForward + GetActorRightVector() * _RandRight;
			_RandLocation += GetActorLocation();
			_RandLocation.Z = 150.f;

			_SpawnBall->TargetLocation = _RandLocation;
		}
	}
	else if(AttackType == EMonsterAttackType::E_Pattern2)
	{
		LOGTEXT_LOG(TEXT("보스 패턴2 스킬 트리거"));

		TArray<AActor*> _IgnoreActor;
		TArray<FHitResult> _HitResults;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetMesh()->GetSocketLocation(TEXT("HammerCenter")) + GetActorForwardVector() * 100.f, GetMesh()->GetSocketLocation(TEXT("HammerCenter")),
			450.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6), false, _IgnoreActor, EDrawDebugTrace::ForDuration, _HitResults, true, FLinearColor::Red);

		for (FHitResult _HitResult : _HitResults)
		{
			APlayableCharacter* _Player = Cast<APlayableCharacter>(_HitResult.Actor);
			if(_Player != nullptr)
			{
				LOGTEXT_LOG(TEXT("보스 패턴2 플레이어 Hit"));
				_Player->GetAttacked(50.f);
			}
		}
	}
	else if(AttackType == EMonsterAttackType::E_Pattern3)
	{
		LOGTEXT_LOG(TEXT("보스 패턴3 스킬 트리거"));

		FActorSpawnParameters _SpawnParams;
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		
		GetWorld()->SpawnActor<ABossPattern3>(Pattern3Actor, Player->GetActorLocation(), FRotator::ZeroRotator, _SpawnParams);
	}
}

void ABossMonsterBase::TrySetHitReaction()
{
	//Super::TrySetHitReaction();
}

void ABossMonsterBase::ChangeMonsterHp(float _HpChangeValue)
{
	//Super::ChangeMonsterHp(_HpChangeValue);
	bool _bDieFlag = false;
	// Hp 작용
	MonsterInfo.CurrentHp -= _HpChangeValue;
	if(MonsterInfo.CurrentHp < 0.f)
	{
		MonsterInfo.CurrentHp = 0.f;
		_bDieFlag = true;
	}

	// HUD 업데이트(플레이어HUD)
	UProgressBar* _HpBar = Cast<UProgressBar>(UDungeonManager::Instance()->PlayerHUD->GetWidgetFromName(TEXT("BossHPBar")));
	float _Percent = MonsterInfo.CurrentHp / MonsterInfo.MaxHp;
	_HpBar->SetPercent(_Percent);


	// 사망 시
	if(_bDieFlag)
	{
		if(MonsterState == EMonsterState::E_Dying)
			return;
		SetMonsterState(EMonsterState::E_Dying);
		bNeverChangableState = true;
	}
}

void ABossMonsterBase::EndDying_FromNotify()
{
	//Super::EndDying_FromNotify();

	// 클리어 스크린
	UDungeonManager::Instance()->ClearBoss();
}
