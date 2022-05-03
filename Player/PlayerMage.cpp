// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMage.h"

#include "PlayerAnimInstance.h"
#include "PortFolio.h"
#include "Combat/ProjectileGeneral.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/MonsterGeneralCharacter.h"

APlayerMage::APlayerMage()
{
	// 메시 찾기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> _MageMesh(TEXT("SkeletalMesh'/Game/ParagonSerath/Characters/Heroes/Serath/Meshes/Serath.Serath'"));
	if(_MageMesh.Succeeded())
		GetMesh()->SetSkeletalMesh(_MageMesh.Object);

	// ABP 찾기
	static ConstructorHelpers::FClassFinder<UAnimInstance> _MageABP(TEXT("AnimBlueprint'/Game/_Animations/Player/ABP_Mage.ABP_Mage_C'"));
	if(_MageABP.Succeeded())
		GetMesh()->SetAnimInstanceClass(_MageABP.Class);

	Class = EPlayerClass::E_Mage;
}

void APlayerMage::BeginPlay()
{
	Super::BeginPlay();

	//PlayerController = Cast<APlayerController>(GetController());
	PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void APlayerMage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCastingSkill3(DeltaSeconds);
}

// 기본 공격
void APlayerMage::ActionTrigger_AttackBasic()
{
	if(AnimInstance->BasicAttackNum != 2)
	{
		// Projectile 3개 소환
		TArray<AProjectileGeneral*> _GenProjectileArray;
		FActorSpawnParameters _SpawnParams = {};
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_SpawnParams.bDeferConstruction = true;

		for(int i = 0; i < 3; i++)
		{
			AProjectileGeneral* _GenProjectile = GetWorld()->SpawnActor<AProjectileGeneral>(ActingInfos.BasicAttackProjectile, _SpawnParams);
			_GenProjectile->SetAttackDamage(ActingInfos.BasicAttackPower);
			_GenProjectile->bDontOverlap = true;
			_GenProjectile->bDontDestroyAuto = true;
			_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());
			_GenProjectile->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			_GenProjectileArray.Add(_GenProjectile);
			BasicProjectiles.Add(_GenProjectile);
		}
		
		// 왼쪽 위치 설정
		if(AnimInstance->BasicAttackNum == 0)
		{
			_GenProjectileArray[0]->SetActorRelativeLocation(FVector(-60.f, -70.f, 60.f));
			_GenProjectileArray[1]->SetActorRelativeLocation(FVector(-60.f, -60.f, 110.f));
			_GenProjectileArray[2]->SetActorRelativeLocation(FVector(-60.f, -40.f, 150.f));
		}
		// 오른쪽 위치 설정
		else
		{
			_GenProjectileArray[0]->SetActorRelativeLocation(FVector(-60.f, 70.f, 60.f));
			_GenProjectileArray[1]->SetActorRelativeLocation(FVector(-60.f, 60.f, 110.f));
			_GenProjectileArray[2]->SetActorRelativeLocation(FVector(-60.f, 40.f, 150.f));
		}
	}
	else
	{
		// 가장 가까운 적 찾기
		FCollisionQueryParams _CollisionParams(NAME_None, false, this);
		TArray<AActor*> _ActorsToIgnore;
		TArray<FHitResult> _HitResults;
		bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 2000.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::None, _HitResults, true);
		// 적 있을 때
		if(_bResult)
		{
			AMonsterGeneralCharacter* _ClosestMonster = Cast<AMonsterGeneralCharacter>(_HitResults[0].Actor);
			float _CurDistance = FVector::Distance(GetActorLocation(), _HitResults[0].Actor->GetActorLocation());
			for(int i = 1; i < _HitResults.Num(); i++)
			{
				if(FVector::Distance(GetActorLocation(), _HitResults[i].Actor->GetActorLocation()) < _CurDistance)
				{
					_ClosestMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
					_CurDistance = FVector::Distance(GetActorLocation(), _HitResults[i].Actor->GetActorLocation());
				}
			}

			// 해당 적에게 유도탄 발사
			for(int i = 0; i < BasicProjectiles.Num(); i++)
			{
				BasicProjectiles[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				BasicProjectiles[i]->GetProjectileMovement()->bIsHomingProjectile = true;
				BasicProjectiles[i]->GetProjectileMovement()->HomingAccelerationMagnitude = 1500.f;
				BasicProjectiles[i]->GetProjectileMovement()->HomingTargetComponent = _ClosestMonster->GetCapsuleComponent();
				BasicProjectiles[i]->SetLifeTime(5.f);
				BasicProjectiles[i]->bDestroyOnHomingNull = true;
				BasicProjectiles[i]->bDontOverlap = false;
				BasicProjectiles[i]->bDontDestroyAuto = false;
				BasicProjectiles[i]->bDestroyOnHit = true;
				BasicProjectiles[i]->GetProjectileMovement()->Activate();
			}
			BasicProjectiles.Empty();
		}
		// 적 없을 때
		else
		{
			// 정면으로 유도탄 발사
			for(int i = 0; i < BasicProjectiles.Num(); i++)
			{
				BasicProjectiles[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				BasicProjectiles[i]->GetProjectileMovement()->InitialSpeed = 1500.f;
				BasicProjectiles[i]->GetProjectileMovement()->Velocity = BasicProjectiles[i]->GetActorForwardVector() * 1000.f;
				BasicProjectiles[i]->SetLifeTime(3.f);
				BasicProjectiles[i]->bDontOverlap = false;
				BasicProjectiles[i]->bDontDestroyAuto = false;
				BasicProjectiles[i]->bDestroyOnHit = true;
				BasicProjectiles[i]->GetProjectileMovement()->Activate();
			}
			BasicProjectiles.Empty();
		}
		
	}
}

void APlayerMage::ActionTrigger_Skill1()
{
	// 충돌 체크
	float _Range = 1000.f;
	float _Radius = 200.f;
	FCollisionQueryParams _CollisionParams(NAME_None, false);
	TArray<AActor*> _ActorsToIgnore;
	TArray<FHitResult> _HitResults;
	bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * _Range, _Radius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::ForDuration, _HitResults, true);
	if(_bResult)
	{
		for(int i = 0; i < _HitResults.Num(); i++)
		{
			AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
			if(_HitMonster != nullptr)
			{
				_HitMonster->TrySetHitReaction();
				_HitMonster->ChangeMonsterHp(ActingInfos.Skill1AttackPower);
			}
		}
	}
}

void APlayerMage::GetActionInput_Skill2()
{
	Super::GetActionInput_Skill2();

	// 확인: 마나
	if(PlayerInfo.CurrentMp < ActingInfos.Skill2MP)
		return;
	ChangeMp(ActingInfos.Skill2MP);

	// 몽타주 플레이
	AnimInstance->Montage_Play(Skill2Montage);
}

void APlayerMage::ActionTrigger_Skill2()
{
	// 8방향으로 Projectile 생성
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.bDeferConstruction = true;
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<AProjectileGeneral*> _GenProjArray;
	for(int i = 0; i < 8; i++)
	{
		AProjectileGeneral* _GenProj = GetWorld()->SpawnActor<AProjectileGeneral>(ActingInfos.Skill2Projectile, GetActorLocation(), FRotator::ZeroRotator, _SpawnParams);
		_GenProj->bDestroyOnHit = true;
		_GenProj->SetAttackDamage(ActingInfos.Skill2AttackPower);
		_GenProj->FinishSpawning(_GenProj->GetTransform());
		_GenProjArray.Add(_GenProj);
	}

	// 방향 설정
	FVector _Forward = GetActorForwardVector();
	FVector _Backward = _Forward * -1.f;
	FVector _Right = GetActorRightVector();
	FVector _Left = _Right * -1.f;
	_GenProjArray[0]->GetProjectileMovement()->Velocity = _Forward.GetSafeNormal() * 1000.f;
	_GenProjArray[1]->GetProjectileMovement()->Velocity = (_Forward + _Right).GetSafeNormal() * 1000.f;
	_GenProjArray[2]->GetProjectileMovement()->Velocity = _Right.GetSafeNormal() * 1000.f;
	_GenProjArray[3]->GetProjectileMovement()->Velocity = (_Right + _Backward).GetSafeNormal() * 1000.f;
	_GenProjArray[4]->GetProjectileMovement()->Velocity = _Backward.GetSafeNormal() * 1000.f;
	_GenProjArray[5]->GetProjectileMovement()->Velocity = (_Backward + _Left).GetSafeNormal() * 1000.f;
	_GenProjArray[6]->GetProjectileMovement()->Velocity = _Left.GetSafeNormal() * 1000.f;
	_GenProjArray[7]->GetProjectileMovement()->Velocity = (_Left + _Forward).GetSafeNormal() * 1000.f;
	
}

void APlayerMage::GetActionInput_AttackBasic()
{
	if(!bIsCasting)
	{   
		if(GetCurrentState() == EPlayerState::E_Dying || GetCurrentState() == EPlayerState::E_Jumping)	return;
	
		// 확인: 쿨타임 중인지
		//if(bBasicAttackCooling)		return;

		// 실행~
		// 공격 유형에 따라 이동 가능 여부 판별
		EPlayerState _ActingState = ActingInfos.BasicAttackMovable ? EPlayerState::E_ActingMoving : EPlayerState::E_ActingStatic;
		// 상태State 설정
		SetCurrentState(_ActingState);
		// Acting Type 설정
		SetCurrentActingType(EActingType::E_AttackBasic);
		// 애니메이션 재생
		AnimInstance->PlayActingAnimation(EActingType::E_AttackBasic);
		// 쿨다운 설정
		//Cooldown_AttackBasic();
	}
	else
		ExecuteSkill3();
}

void APlayerMage::GetActionInput_Skill3()
{
	if(ActingInfos.bSkill3Locked)
		return;
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)
		return;
	if(bSkill3Cooling)
		return;

	// 확인: 마나
	if(PlayerInfo.CurrentMp < ActingInfos.Skill3MP)
		return;
	ChangeMp(ActingInfos.Skill3MP);
	
	Super::GetActionInput_Skill3();

	// 애니메이션 시작
	AnimInstance->Montage_Play(Skill3Montage);

	// 카메라 회전 방지
	bCastingIgnoreRot = true;
}

void APlayerMage::ActionTrigger_Skill3()
{
	// 캐스팅 시작
	bIsCasting = true;
	// Decal 생성
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = true;

	CastingDecalActor = GetWorld()->SpawnActor<AActor>(ActingInfos.CastingDecalActorClass, GetActorLocation(), FRotator(-90.f, 0.f, 0.f), _SpawnParams);
	CastingDecalActor->FinishSpawning(CastingDecalActor->GetTransform());
	CastingDecalActor->SetActorScale3D(FVector(3.f, 3.f, 3.f));
}

void APlayerMage::TickCastingSkill3(float _DeltaTime)
{
	if(bIsCasting)
	{
		if(PlayerController == nullptr)
			PlayerController = Cast<APlayerController>(GetController());
		
		// 충돌 지점에 Decal Actor 위치시키기
		FHitResult _HitResult = {};
		bool _Hit = PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel8, false, _HitResult);
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

void APlayerMage::ExecuteSkill3()
{
	bIsCasting = false;

	// Decal 회전 멈추기
	URotatingMovementComponent* _RotatingComp = Cast<URotatingMovementComponent>(CastingDecalActor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
	_RotatingComp->Deactivate();

	// 다음 몽타주 시작
	AnimInstance->Montage_JumpToSection(TEXT("Execute"), Skill3Montage);
}

void APlayerMage::MiscTrigger1()
{
	// 생성 위치 설정
	float _RandRange = 600.f;
	FVector _RandLocation = CastingDecalActor->GetActorLocation() + FVector(FMath::FRandRange(-_RandRange, _RandRange), FMath::FRandRange(-_RandRange, _RandRange), 1000.f);
	
	// 메테오 생성
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParam.bDeferConstruction = true;

	AProjectileGeneral* _GenProj = GetWorld()->SpawnActor<AProjectileGeneral>(ActingInfos.Skill3Projectile, _RandLocation, FRotator::ZeroRotator, _SpawnParam);
	_GenProj->SetAttackDamage(ActingInfos.Skill3AttackPower);
	_GenProj->bHitOnTerrain = true;
	_GenProj->GetProjectileMovement()->ProjectileGravityScale = FMath::FRandRange(0.4f, 1.f);
	_GenProj->FinishSpawning(_GenProj->GetTransform());
}

void APlayerMage::MiscTrigger2()
{
	// 캐스팅 Actor 제거
	CastingDecalActor->Destroy();
}

void APlayerMage::EndActingFromNotify()
{
	// 행동(공격, 회피 등) 완료시 Notify(애님인스턴스)에서 호출
	// 콤보중일 때
	bCanBasicAttackCombo = false;
	if(AnimInstance->BasicAttackNum == 2)
		AnimInstance->BasicAttackNum = 0;
	else
		AnimInstance->BasicAttackNum++;
	SetCurrentState(EPlayerState::E_Idle);
	SetCurrentActingType(EActingType::E_None);
	// 무적 상태 종료
	if(GetIsInvincible())	SetIsInvincible(false);
	// Casting 종료, 회전가능
	if(bCastingIgnoreRot)
		bCastingIgnoreRot = false;
}
