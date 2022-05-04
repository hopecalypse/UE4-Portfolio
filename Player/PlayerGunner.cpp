// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerGunner.h"
#include "PlayerAnimInstance.h"
#include "PortFolio.h"
#include "Combat/ProjectileGeneral.h"
#include "Combat/Gunner/Gunner_Grenade.h"
#include "Combat/Gunner/Gunner_Missile.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/AudioDataAsset.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/SoundManager.h"
#include "Monster/MonsterGeneralCharacter.h"

APlayerGunner::APlayerGunner()
{
	// 메시 찾기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> _GunnerMesh(TEXT("SkeletalMesh'/Game/ParagonRevenant/Characters/Heroes/Revenant/Meshes/Revenant.Revenant'"));
	if(_GunnerMesh.Succeeded())
		GetMesh()->SetSkeletalMesh(_GunnerMesh.Object);

	// 애님인스턴스 ABP 찾기
	static ConstructorHelpers::FClassFinder<UPlayerAnimInstance> _GunnerABP(TEXT("AnimBlueprint'/Game/_Animations/Player/ABP_Gunner.ABP_Gunner_C'"));
	if(_GunnerABP.Succeeded())
		GetMesh()->SetAnimInstanceClass(_GunnerABP.Class);

	// 에임 타겟 WBP 찾기
	static ConstructorHelpers::FClassFinder<AActor> _TargetBP(TEXT("Blueprint'/Game/_Blueprints/Combat/Gunner/BP_EnemyTargetUI.BP_EnemyTargetUI_C'"));
	if(_TargetBP.Succeeded())
		TargetActorBP = _TargetBP.Class;


	Class = EPlayerClass::E_Gunner;
	
	// Gun Point 만들기
	GunPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GunPoint"));
	GunPoint->SetupAttachment(GetMesh(), FName(TEXT("GunPointSocket")));
	// Righthand Point 만들기
	RighthandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandPoint"));
	RighthandPoint->SetupAttachment(GetMesh(), FName(TEXT("RightHandSocket")));

	// Enemy TargetBox 만들기
	EnemyTargetBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Enemy Target Box"));
	EnemyTargetBox->SetupAttachment(RootComponent);
	EnemyTargetBox->SetRelativeLocation(FVector(1000.f, 0.f, 100.f));
	EnemyTargetBox->SetBoxExtent(FVector(1000.f, 300.f, 32.f));
	EnemyTargetBox->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	EnemyTargetBox->SetCollisionProfileName(TEXT("PlayerProjectile"));
}

void APlayerGunner::BeginPlay()
{
	Super::BeginPlay();

	// 적 감지 박스에 델리게이트 설정
	EnemyTargetBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerGunner::BoxOnOverlapEnemy);
	EnemyTargetBox->OnComponentEndOverlap.AddDynamic(this, &APlayerGunner::BoxEndOverlapEnemy);

	// 타겟 액터 생성
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParam.bDeferConstruction = true;

	TargetUIActor = GetWorld()->SpawnActor(TargetActorBP);
	TargetWidgetComponent = Cast<UWidgetComponent>(TargetUIActor->GetComponentByClass(UWidgetComponent::StaticClass()));
	TargetWidgetComponent->SetVisibility(false);
}

void APlayerGunner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTargetingEnemy();
	if(bOnSkill3)
	{
		Skill3ExecuteAccTime += DeltaSeconds;
		if(Skill3ExecuteAccTime > Skill3Interval)
			ExecuteSkill3();
		
		Skill3AccTime += DeltaSeconds;
		if(Skill3AccTime > Skill3LengthTime)
			EndSkill3();
	}
}

void APlayerGunner::GetActionInput_AttackBasic()
{
	//Super::GetActionInput_AttackBasic();

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

void APlayerGunner::ActionTrigger_AttackBasic()
{
	// 소켓 위치(GunPoint)에 생성
	AProjectileGeneral* _GenBasic = SpawnProjectileToGun(ActingInfos.BasicAttackProjectile, ActingInfos.BasicAttackProjectileLifeTime, ActingInfos.BasicAttackPower);
	
	// 타겟팅 적이 있을 때
	if(TargetingEnemy != nullptr)
	{
		_GenBasic->GetProjectileMovement()->bIsHomingProjectile = true;
		_GenBasic->GetProjectileMovement()->HomingTargetComponent = TargetingEnemy->GetCapsuleComponent();
		_GenBasic->GetProjectileMovement()->HomingAccelerationMagnitude = 2000.f;
		_GenBasic->GetProjectileMovement()->Velocity = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetingEnemy->GetActorLocation()).Vector() * 2000.f;
		_GenBasic->SetLifeSpan(2.f);
	}
	// 타겟팅 적이 없을 때
	else
	{
		// 정면으로 방향 설정
		_GenBasic->GetProjectileMovement()->Velocity = GetActorForwardVector() * 2000.f;
	}
}

// Gun 위치 에서 Projectle 생성하기
AProjectileGeneral* APlayerGunner::SpawnProjectileToGun(TSubclassOf<AProjectileGeneral> _ProjectileClass, float _LifeTime, float _AttackDamage)
{
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParam.bDeferConstruction = false;

	AProjectileGeneral* _GenProjectile = GetWorld()->SpawnActor<AProjectileGeneral>(_ProjectileClass, GunPoint->GetComponentLocation() - FVector(0.f, 0.f, 80.f), FRotator::ZeroRotator);
	//_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());
	_GenProjectile->SetLifeTime(_LifeTime);
	_GenProjectile->SetAttackDamage(_AttackDamage);

	return _GenProjectile;
}

// 스킬1 함수 오버라이드
void APlayerGunner::ActionTrigger_Skill1()
{
	// 오른손 포인트(소켓) 에 수류탄 생성하기
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParam.bDeferConstruction = false;

	AGunner_Grenade* _GenProjectile = GetWorld()->SpawnActor<AGunner_Grenade>(ActingInfos.Skill1Projectile, RighthandPoint->GetComponentLocation(), FRotator::ZeroRotator);
	//_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());
	_GenProjectile->SetAttackDamage(ActingInfos.Skill1AttackPower);
	// 방향 설정
	FVector _Direction = GetActorForwardVector() * 800.f;
	if(TargetingEnemy != nullptr)
		_Direction = TargetingEnemy->GetActorLocation() - GetActorLocation();
		//_Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetingEnemy->GetActorLocation()).Vector();
	_GenProjectile->InitGrenade(_Direction, ActingInfos.Skill1AttackPower);
}

void APlayerGunner::GetActionInput_Skill2()
{
	if(ActingInfos.bSKill2Locked)
		return;
	// 재정의 목적: 타겟이 있을때만 실행하기 위해
	
	// 확인: State로 가능한지 판별(Idle, Move일때만)
    if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)	return;
    // 확인: 쿨타임 중인지
    if(bSkill2Cooling)	return;
	//! 확인: 타겟이 있는지
	if(TargetingEnemy == nullptr)
	{
		LOGTEXT_LOG(TEXT("Gunner 스킬2: 타겟이 있어야 사용 가능"));
		return;
	}
	// 확인: 마나
	if(PlayerInfo.CurrentMp < ActingInfos.Skill2MP)
		return;
	ChangeMp(ActingInfos.Skill2MP);
	// 타겟 저장해놓기
	SavedTargetForMissile = TargetingEnemy; 

    // 실행~
    // 공격 유형에 따라 이동 가능 여부 판별
    EPlayerState _ActingState = ActingInfos.Skill2AttackMovable ? EPlayerState::E_ActingMoving : EPlayerState::E_ActingStatic;
    // 상태State 설정
    SetCurrentState(_ActingState);
    // Acting Type 설정
    SetCurrentActingType(EActingType::E_AttackSkill2);
    // 애니메이션 재생
    AnimInstance->PlayActingAnimation(EActingType::E_AttackSkill2);
    // 쿨다운 설정
    Cooldown_Skill2();
}

// 스킬2 실행
void APlayerGunner::ActionTrigger_Skill2()
{
	//Super::ActionTrigger_Skill2();

	// 미사일 8개 생성하기
	LeftMissileCount = 8;
	SpawnLeftMissiles_Skill2();
}

void APlayerGunner::SpawnLeftMissiles_Skill2()
{
	// 미사일 생성
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//_SpawnParam.bDeferConstruction = true;
	FVector _LocationOffset = FVector(-10.f, 0.f, 80.f);
	
	AGunner_Missile* _GenMissile = Cast<AGunner_Missile>(GetWorld()->SpawnActor<AGunner_Missile>(ActingInfos.Skill2Projectile, GetActorLocation() + _LocationOffset, FRotator::ZeroRotator, _SpawnParam));
	//_GenMissile->FinishSpawning(_GenMissile->GetTransform());
	if(SavedTargetForMissile != nullptr)
		_GenMissile->InitMissile(SavedTargetForMissile->GetRootComponent(), ActingInfos.Skill2AttackPower);

	// 효과음 재생
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Gunner_RocketLaunch);
	
	LeftMissileCount--;
	// 반복 타이머 설정
	if(LeftMissileCount > 0)
	{
		float _Interval = 0.08f;
		FTimerHandle _TimerHandle;
		FTimerDelegate _TimerDelegate;
		_TimerDelegate.BindUFunction(this, FName("SpawnLeftMissiles_Skill2"));
		GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, _Interval, false);
	}
}

void APlayerGunner::GetActionInput_Skill3()
{
	if(ActingInfos.bSkill3Locked)
		return;
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)
		return;
	if(bSkill3Cooling)
		return;

	// 확인: 마나
	if(PlayerInfo.CurrentMp < ActingInfos.Skill2MP)
		return;
	ChangeMp(ActingInfos.Skill2MP);
		
	Super::GetActionInput_Skill3();
	
	// 애니메이션 재생
	AnimInstance->Montage_Play(Skill3Montage);
}

void APlayerGunner::ActionTrigger_Skill3()
{
	if(Skill3Laser != nullptr)
		return;
	
	// 레이저 생성
	FActorSpawnParameters _SpawnParam = {};
	_SpawnParam.OverrideLevel = GetLevel();
	_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//_SpawnParam.bDeferConstruction = true;

	Skill3Laser = GetWorld()->SpawnActor<AActor>(Skill3LaserClass, _SpawnParam);
	//Skill3Laser->FinishSpawning(Skill3Laser->GetTransform());
	
	Skill3Laser->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	Skill3Laser->SetActorRelativeLocation(FVector(160.f, 55.f, 60.f));
	Skill3Laser->SetActorRelativeRotation(FRotator(0.f, -90.f, 0.f));


	bOnSkill3 = true;
}

void APlayerGunner::MiscTrigger1()
{
	// 애니메이션 재생
	AnimInstance->Montage_Play(Skill3Montage);
	AnimInstance->Montage_JumpToSection(TEXT("Looping"));
}

void APlayerGunner::ExecuteSkill3()
{
	Skill3ExecuteAccTime = 0.f;

	if(Skill3Laser == nullptr)
		return;

	// 효과음 재생
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->Gunner_LaserLoop);
	
	// 범위 설정
	FVector _Start = Skill3Laser->GetActorLocation();
	FVector _End = _Start + Skill3Laser->GetActorRightVector() * 1200.f;
	float _Size = 60.f;

	// 레이저 범위만큼 Sweep Check
	FCollisionQueryParams _CollisionParam(NAME_None);
	TArray<AActor*> _ActorsToIgnore;
	TArray<FHitResult> _HitResults;
	bool _bResult = UKismetSystemLibrary::SphereTraceMulti(this, _Start, _End, _Size, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), false, _ActorsToIgnore, EDrawDebugTrace::None, _HitResults, true);
	if(_bResult)
	{
		for(int i = 0; i < _HitResults.Num(); i++)
		{
			AMonsterGeneralCharacter* _HitMonster = Cast<AMonsterGeneralCharacter>(_HitResults[i].Actor);
			if(_HitMonster != nullptr)
			{
				_HitMonster->ChangeMonsterHp(ActingInfos.Skill3AttackPower);
				_HitMonster->TrySetHitReaction();
				FActorSpawnParameters _SpawnParam = {};
				_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				_SpawnParam.OverrideLevel = GetLevel();
				_SpawnParam.bDeferConstruction = true;
				AActor* _LaserHitActor = GetWorld()->SpawnActor<AActor>(Skill3LaserHitClass, _HitResults[i].ImpactPoint, FRotator::ZeroRotator, _SpawnParam);
				//LOGTEXT_LOG(TEXT("%s"), *_HitResults[i].ImpactPoint.ToString())
				_LaserHitActor->FinishSpawning(_LaserHitActor->GetTransform());
				_LaserHitActor->SetActorLocation(_HitResults[i].ImpactPoint);

				UGameplayStatics::PlaySoundAtLocation(GetWorld(), USoundManager::Instance()->Data->Gunner_LaserHit, _HitResults[i].Location, FRotator::ZeroRotator, 0.6f, 1.f, 0.f, USoundManager::Instance()->Data->Attenuation);
			}
		}
	}
}

void APlayerGunner::EndSkill3()
{
	bOnSkill3 = false;
	Skill3AccTime = 0.f;
	Skill3ExecuteAccTime = 0.f;
	
	AnimInstance->Montage_JumpToSection(TEXT("LoopEnd"), Skill3Montage);

	Skill3Laser->Destroy();
	Skill3Laser = nullptr;
}

void APlayerGunner::EndActingFromNotify()
{
	//Super::EndActingFromNotify();

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

// 적 박스 등록
void APlayerGunner::BoxOnOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMonsterGeneralCharacter* _OverlapEnemy = Cast<AMonsterGeneralCharacter>(OtherActor);
	if(_OverlapEnemy != nullptr)
		TargetEnemyList.Add(_OverlapEnemy);
}
// 적 박스 해제
void APlayerGunner::BoxEndOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMonsterGeneralCharacter* _OverlapEnemy = Cast<AMonsterGeneralCharacter>(OtherActor);
	if(_OverlapEnemy != nullptr)
		TargetEnemyList.Remove(_OverlapEnemy);
}

// 가장 가까운 적 표적 구하기
AMonsterGeneralCharacter* APlayerGunner::GetNearestEnemy()
{
	if(TargetEnemyList.Num() == 0)
		return nullptr;
	if(TargetEnemyList.Num() == 1)
		return TargetEnemyList[0];

	// 거리 비교
	AMonsterGeneralCharacter* _Nearest = TargetEnemyList[0];
	FVector _PlayerLoc = GetActorLocation();
	for (auto _Enemy : TargetEnemyList)
	{
		if(FVector::Distance(_PlayerLoc, _Nearest->GetActorLocation()) > FVector::Distance(_PlayerLoc, _Enemy->GetActorLocation()))
			_Nearest = _Enemy;
	}
	
	return _Nearest;
}

// 가장 가까운 적으로 표적 정하기
void APlayerGunner::UpdateTargetingEnemy()
{
	// 만약 죽은 Enemy라면 빼기
	AMonsterGeneralCharacter* _RemoveEnemy = nullptr;
	for (auto _Enemy : TargetEnemyList)
	{
		if(_Enemy->GetMonsterState() == EMonsterState::E_Dying)
			_RemoveEnemy = _Enemy;
	}
	if(_RemoveEnemy != nullptr)
		TargetEnemyList.Remove(_RemoveEnemy);

	// UI 업데이트
	TargetingEnemy = GetNearestEnemy();
	if(TargetingEnemy != nullptr)
	{
		TargetUIActor->SetActorLocation(TargetingEnemy->GetActorLocation());
		TargetWidgetComponent->SetVisibility(true);
	}
	else
	{
		TargetWidgetComponent->SetVisibility(false);
	}
}
