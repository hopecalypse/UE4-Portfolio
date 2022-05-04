// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWarrior.h"

#include "DrawDebugHelpers.h"
#include "PlayerAnimInstance.h"
#include "PortFolio.h"
#include "Combat/ProjectileGeneral.h"
#include "Components/CapsuleComponent.h"
#include "Core/AudioDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/EffectManager.h"
#include "Manager/SoundManager.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Sound/SoundCue.h"

APlayerWarrior::APlayerWarrior()
{
	// 캐릭터 메시 에셋 넣어주기
	ConstructorHelpers::FObjectFinder<USkeletalMesh> WarriorMesh(TEXT("SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));
	if(WarriorMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(WarriorMesh.Object);
	}

	// Anim Instance BP 에셋 넣어주기
	ConstructorHelpers::FClassFinder<UAnimInstance> WarriorABP(TEXT("AnimBlueprint'/Game/_Animations/Player/ABP_Warrior.ABP_Warrior_C'"));
	if(WarriorABP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WarriorABP.Class);
	}

	Class = EPlayerClass::E_Warrior;
}

void APlayerWarrior::BeginPlay()
{
	Super::BeginPlay();

	//PlayerController = Cast<APlayerController>(GetController());
	PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void APlayerWarrior::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCastingSkill3(DeltaSeconds);
	TickFlySkill3(DeltaSeconds);
	TickRotatingSkill2(DeltaSeconds);
}

void APlayerWarrior::ActionTrigger_AttackBasic()
{
	Super::ActionTrigger_AttackBasic();
	// 소리 재생
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Warrior_SwordQueue);
}

void APlayerWarrior::GetActionInput_Skill1()
{
	Super::GetActionInput_Skill1();

	bUseControllerRotationYaw = false;

	// 소리 재생
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Warrior_Skill1Impact);
}

// 스킬1: 땅 찍으며 범위 안 적들에게 데미지
void APlayerWarrior::ActionTrigger_Skill1()
{
	// 1. BP 액터 생성
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = true;
	AProjectileGeneral* _GenProjectile = GetWorld()->SpawnActor<AProjectileGeneral>(ActingInfos.Skill1Projectile, GetActorLocation() + GetActorForwardVector() * 50.f, FRotator::ZeroRotator, _SpawnParams);

	_GenProjectile->SetLifeTime(3.f);
	_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());

	// 2. 공격 Sweep 체크
	TArray<FHitResult> _HitResults;
	TArray<AActor*> _ActorsToIgnore;
	FCollisionQueryParams _Params(NAME_None, false, this);
	bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, _GenProjectile->GetActorLocation(), _GenProjectile->GetActorLocation(), ActingInfos.Skill1MeleeRange, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::None, _HitResults, true);
	if(_bResult)
	{
		for(int i = 0; i < _HitResults.Num(); i++)
		{
			AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
			_HitMonster->TrySetHitReaction();
			_HitMonster->ChangeMonsterHp(ActingInfos.Skill1AttackPower);
		}
	}

	// 3. 다시 회전 가능
	bUseControllerRotationYaw = true;
}

void APlayerWarrior::GetActionInput_Skill2()
{
	if(ActingInfos.bSKill2Locked)
		return;
	Super::GetActionInput_Skill2();

	// 회전 불가
	bCastingIgnoreRot = true;
	bUseControllerRotationYaw = false;
	// 애니메이션 플레이
	AnimInstance->Montage_Play(Skill2Montage);
	// 효과음
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Warrior_Skill2Voice);
}

void APlayerWarrior::ActionTrigger_Skill2()
{
	Super::ActionTrigger_Skill2();
	// 돌진 전 위치 저장
	BeforeDashLocation = GetActorLocation();
	// 앞의 지점으로 나아가며 그 경로 사이에 있는 적들에게 데미지
	LaunchCharacter(GetActorForwardVector() * 10000.f, true, false);
	// 첫 포즈 이펙트 설정
	UEffectManager::Instance()->PoseEffectOnce(this, 5.f);
	// 포즈 이펙트 설정
	UEffectManager::Instance()->SetPoseEffectLoop(true, this, 0.01f, 0.01f);
	// 효과음
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Warrior_Skill2);
}

void APlayerWarrior::TickRotatingSkill2(float _DeltaTime)
{
	if(!bDashRotating)
		return;
	
	// Before Location으로 회전하기
	FRotator _LookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), BeforeDashLocation);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), _LookRotator, _DeltaTime, 20.f));
	//DrawDebugSphere(GetWorld(), BeforeDashLocation, 10.f, 10, FColor::Blue, false, 5.f);
}

void APlayerWarrior::GetActionInput_Skill3()
{
	if(ActingInfos.bSkill3Locked)
		return;
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)
		return;
	if(bSkill3Cooling)
		return;
	
	Super::GetActionInput_Skill3();
	
	// 애니메이션 시작
	AnimInstance->Montage_Play(Skill3Montage);

	// 카메라 회전 방지
	bCastingIgnoreRot = true;
}

void APlayerWarrior::ActionTrigger_Skill3()
{
	// Decal 생성
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = true;

	CastingDecalActor = GetWorld()->SpawnActor<AActor>(ActingInfos.CastingDecalActorClass, GetActorLocation(), FRotator(-90.f, 0.f, 0.f), _SpawnParams);
	CastingDecalActor->FinishSpawning(CastingDecalActor->GetTransform());
	CastingDecalActor->SetActorScale3D(FVector(2.f, 2.f, 2.f));

	// 영역 선택 시작
	bIsCasting = true;
}

void APlayerWarrior::TickCastingSkill3(float _DeltaTime)
{
	if(bIsCasting)
	{
		if(PlayerController == nullptr)
			PlayerController = Cast<APlayerController>(GetController());
	
		// 충돌 지점에 Decal Actor 위치시키기
		FHitResult _HitResult = {};
		bool _Hit = PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel8), false, _HitResult);
		//bool _Hit = PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel8, false, _HitResult);
		if(_Hit == false)
			return;

		CastingDecalActor->SetActorLocation(_HitResult.ImpactPoint);
		
		// ~. 계산된 각도에 따라 캐릭터 회전 ~
		// 1. 마우스 위치, 화면 해상도 찾기
		float _MouseX, _MouseY, _ResX, _ResY;
		int32 _iResX, _iResY;
		PlayerController->GetMousePosition(_MouseX, _MouseY);
		PlayerController->GetViewportSize(_iResX, _iResY);
		_ResX = (float)_iResX;
		_ResY = (float)_iResY;

		// 마우스가 중앙으로부터 얼마나 멀어졌는지 비율 절대값
		float _Ratio = fabs((_MouseX - (_ResX / 2)) / (_ResX / 2));

		// 화면 중심으로부터 30% 이상 멀어졌을 때 캐릭터 회전
		if(_Ratio > 0.3f)
		{
			// 왼쪽, 오른쪽 구분
			float _RotationValue = 0.f;
			if(_MouseX < _ResX / 2)
				_RotationValue = -1.f;
			else
				_RotationValue = 1.f;
			
			// 화면 초과 비율만큼 속도 할당하기(50%일 때 최대속도)
			float _ResultSpeed = _Ratio * 50.f;

			// 최종 계산
			_ResultSpeed = _ResultSpeed * _RotationValue * _DeltaTime;
			
			// 캐릭터 회전
			PlayerController->AddYawInput(_ResultSpeed);
		}
	}
}

void APlayerWarrior::GetActionInput_AttackBasic()
{
	if(!bIsCasting)
	{
		//Super::GetActionInput_AttackBasic();
		if(GetCurrentState() == EPlayerState::E_Dying || GetCurrentState() == EPlayerState::E_Jumping)	return;

		// 공격중일 때-> 콤보 가능하면 실행
		if((GetCurrentState() == EPlayerState::E_ActingMoving || GetCurrentState() == EPlayerState::E_ActingStatic) && bCanBasicAttackCombo)
		{
			AnimInstance->BasicAttackNum++;
			if(AnimInstance->BasicAttackNum != 2)
				bDontEndBasicAttack = true;
			else
				bDontEndBasicAttack = false;
			return;
		}

		// 실행~
		// 공격 유형에 따라 이동 가능 여부 판별
		EPlayerState _ActingState = ActingInfos.BasicAttackMovable ? EPlayerState::E_ActingMoving : EPlayerState::E_ActingStatic;
		// 상태State 설정
		SetCurrentState(_ActingState);
		// Acting Type 설정
		SetCurrentActingType(EActingType::E_AttackBasic);
		// 애니메이션 재생
		AnimInstance->PlayActingAnimation(EActingType::E_AttackBasic);
	}
	else
		Execute_Skill3();
}

void APlayerWarrior::Execute_Skill3()
{
	bIsCasting = false;

	// Decal 회전 멈추기
	URotatingMovementComponent* _RotatingComp = Cast<URotatingMovementComponent>(CastingDecalActor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
	_RotatingComp->Deactivate();

	// 위치 저장
	SelectLocation = CastingDecalActor->GetActorLocation();
	
	// 선택, 캐릭터 중간 위치로 날기
	GetCharacterMovement()->GravityScale = 0.f;
	FlyingLocation = FVector((GetActorLocation().X + SelectLocation.X / 2), (GetActorLocation().Y + SelectLocation.Y / 2), GetActorLocation().Z);
	SelectDistance = FVector::Distance(GetActorLocation(), SelectLocation);
    bIsFlying = true;
	
	// 애니메이션 몽타주 실행(Execute)
	AnimInstance->Montage_JumpToSection(TEXT("Execute"), Skill3Montage);
}

void APlayerWarrior::TickFlySkill3(float _DeltaTime)
{
	if(bIsFlying)
	{
		if(PlayerController == nullptr)
			PlayerController = Cast<APlayerController>(GetController());
		
		// 위치로 이동하기
		FVector _FlyingVector = FMath::VInterpTo(GetActorLocation(), FlyingLocation, _DeltaTime, SelectDistance / 500.f);
		SetActorLocation(_FlyingVector);

		// 방향으로 카메라 회전
		FRotator _LookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FlyingLocation);
		_LookRotator.Pitch = 0.f;
		_LookRotator.Roll = 0.f;
		FRotator _InterpRotator = FMath::RInterpTo(GetActorRotation(), _LookRotator, _DeltaTime, 1.f);
		PlayerController->SetControlRotation(_InterpRotator);
	}
}

void APlayerWarrior::MiscTrigger1()
{
	// 스킬3중일 때
	if(AnimInstance->Montage_IsPlaying(Skill3Montage))
	{
		// 선택 위치로 착지
		SelectDistance *= 100.f;
		FlyingLocation = FVector(SelectLocation.X, SelectLocation.Y, SelectLocation.Z + GetDefaultHalfHeight());
	}
	// 스킬2일 때
	else if(AnimInstance->Montage_IsPlaying(Skill2Montage))
	{
		// 포즈 이펙트 해제
		UEffectManager::Instance()->SetPoseEffectLoop(false);
		// 돌진 후, 돌진 전 위치 ~ 지금 위치 Sweep 체크
		TArray<FHitResult> _HitResults;
		TArray<AActor*> _ActorsToIgnore;
		FCollisionQueryParams _Params(NAME_None, false, this);
		bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, BeforeDashLocation, GetActorLocation(), ActingInfos.Skill2MeleeRange, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::ForDuration, _HitResults, true);
		if(_bResult)
		{
			for(int i = 0; i < _HitResults.Num(); i++)
			{
				AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
				_HitMonster->TrySetHitReaction();
				_HitMonster->ChangeMonsterHp(ActingInfos.Skill3AttackPower);
			}
		}
		// 회전 시작
		bDashRotating = true;
	}
}

void APlayerWarrior::MiscTrigger2()
{
	// 스킬3일 때
	if(AnimInstance->Montage_IsPlaying(Skill3Montage))
	{
		// 조작 원상복구
		bIsFlying = false;
		bCastingIgnoreRot = false;
		GetCharacterMovement()->GravityScale = 1.f;
	
		// 공격 Sweep 체크
		TArray<FHitResult> _HitResults;
		TArray<AActor*> _ActorsToIgnore;
		FCollisionQueryParams _Params(NAME_None, false, this);
		bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), ActingInfos.Skill3MeleeRange, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::None, _HitResults, true);
		if(_bResult)
		{
			for(int i = 0; i < _HitResults.Num(); i++)
			{
				AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
				_HitMonster->TrySetHitReaction();
				_HitMonster->ChangeMonsterHp(ActingInfos.Skill3AttackPower);
			}
		}
	
		// Decal 액터 삭제
		CastingDecalActor->Destroy();	
	}
	// 스킬2일 때
	if(AnimInstance->Montage_IsPlaying(Skill2Montage))
	{
		Skill2Count++;
		if(Skill2Count < 2)
		{
			// 다시 반복하기
			AnimInstance->Montage_Play(Skill2Montage);
		}
		// 3회 반복시 -> 종료하기
		else
		{
			Skill2Count = 0;
			// 컨트롤 가능
			bDashRotating = false;
			bCastingIgnoreRot = false;
			bUseControllerRotationYaw = true;
			EndActingFromNotify();
		}
	}
}
