// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterGeneralCharacter.h"

#include "DrawDebugHelpers.h"
#include "MonsterAnimInstance.h"
#include "PortFolio.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Monster/AI/MonsterAIController.h"
#include "Player/PlayableCharacter.h"
#include "UI/MonsterHpBar.h"


AMonsterGeneralCharacter::AMonsterGeneralCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 생성
	HpbarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hp Bar"));
	static ConstructorHelpers::FClassFinder<UUserWidget> _BPHPbar(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_MonsterHPBar.WBP_MonsterHPBar_C'"));
	if(_BPHPbar.Succeeded())
		HpbarWidgetComponent->SetWidgetClass(_BPHPbar.Class);

	// 컴포넌트 설정
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("EnemyCharacter"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	HpbarWidgetComponent->SetupAttachment(GetMesh());
	HpbarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HpbarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	
	
	// AI 컨트롤러 지정, 빙의 방식 지정
	AIControllerClass = AMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// State 초기값 설정
	SetMonsterState(EMonsterState::E_Idle);
}

void AMonsterGeneralCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 애님인스턴스 찾아오기
	MonsterAnimInstance = Cast<UMonsterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AMonsterGeneralCharacter::BeginPlay()
{
	Super::BeginPlay();

	// HPBar 오브젝트 찾기
	HpBarWidgetObject = Cast<UMonsterHpBar>(HpbarWidgetComponent->GetUserWidgetObject());
	// HPBar 체력 초기화
	HpBarWidgetObject->UpdateHpBar(MonsterInfo.CurrentHp, MonsterInfo.MaxHp);
}

void AMonsterGeneralCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//LOGTEXT_LOG(TEXT("%d"), GetMonsterState());
}

void AMonsterGeneralCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMonsterGeneralCharacter::SetMonsterState(EMonsterState _Value)
{
	if(bNeverChangableState)	return;
	MonsterState = _Value;
}

void AMonsterGeneralCharacter::ChangeMonsterHp(float _HpChangeValue)
{
	bool _bDieFlag = false;
	// Hp 작용
	MonsterInfo.CurrentHp -= _HpChangeValue;
	if(MonsterInfo.CurrentHp < 0.f)
	{
		MonsterInfo.CurrentHp = 0.f;
		_bDieFlag = true;
	}
	//LOGTEXT_LOG(TEXT("몬스터 데미지: %f, 현재 체력: %f"), _HpChangeValue, MonsterInfo.CurrentHp);
	// Hp 업데이트
	HpBarWidgetObject->UpdateHpBar(MonsterInfo.CurrentHp, MonsterInfo.MaxHp);
	if(_bDieFlag)
		DieImmediately();
}

void AMonsterGeneralCharacter::DieImmediately()
{
	//LOGTEXT_WARN(TEXT("몬스터 Die"));
	
	// 충돌 불가
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	HpbarWidgetComponent->SetVisibility(false);
	SetMonsterState(EMonsterState::E_Dying);
	bNeverChangableState = true;
}

void AMonsterGeneralCharacter::SetMovementSpeed(float _Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = _Speed;
}

// * 애니메이션 관련
void AMonsterGeneralCharacter::AttackTrigger_FromNotify()
{
	if(GetMonsterAttackInfo().IsMeleeSweep)
	{
		// 공격 성공 Check
		FHitResult _HitResult;
		FCollisionQueryParams _Params(NAME_None, false, this);
		float _AttackRange = 200.f;
		float _AttackRadius = 70.f;

		bool _bResult = GetWorld()->SweepSingleByChannel(OUT _HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * _AttackRange,
			FQuat::Identity, ECollisionChannel::ECC_EngineTraceChannel5, FCollisionShape::MakeSphere(_AttackRadius), _Params);

	
		// 타격 결과 O + 플레이어일 때
		if(_bResult)
		{
			if(Cast<APlayableCharacter>(_HitResult.Actor) != nullptr)
			{
				Cast<APlayableCharacter>(_HitResult.Actor)->GetAttacked(GetMonsterInfo().AttackPoint);
			}
		}
	
		// 디버그 구체 그리기
		#ifdef ENABLE_DRAW_DEBUG
		FVector _Vec = GetActorForwardVector() * _AttackRange;	// 방향 벡터
		FVector _Center = GetActorLocation() + _Vec * 0.5f;
		float _HalfHegiht = _AttackRange * 0.5f + _AttackRadius;
		DrawDebugCapsule(GetWorld(),
			_Center,
			_HalfHegiht,
			_AttackRadius,
			FRotationMatrix::MakeFromZ(_Vec).ToQuat(),
			_bResult == true ? FColor::Red : FColor::White,
			false,
			1.f);
#endif
	}
	
	// Projectile 있다면 생성하기
	if(GetMonsterAttackInfo().AttackProjectile != nullptr)
	{
		// BP 액터 생성 Param
		FVector _ForwardVector = GetActorForwardVector();
		FActorSpawnParameters _SpawnParams = {};
		_SpawnParams.OverrideLevel = GetLevel();
		_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_SpawnParams.bDeferConstruction = true;
		AEnemyProjectile* _GenProjectile = GetWorld()->SpawnActor<AEnemyProjectile>(GetMonsterAttackInfo().AttackProjectile, GetActorLocation() + _ForwardVector * 50.f, _ForwardVector.Rotation(), _SpawnParams);
		if(_GenProjectile == nullptr)
		{
			LOGTEXT_ERROR(TEXT("Enemy 발사체 생성 실패!"));
			return;
		}
		// 발사체 이동 설정
		_GenProjectile->SetAttackPower(GetMonsterInfo().AttackPoint);
		_GenProjectile->GetProjectileMovement()->Velocity = _ForwardVector;
		_GenProjectile->GetProjectileMovement()->InitialSpeed = _GenProjectile->MoveSpeed;
		_GenProjectile->GetProjectileMovement()->MaxSpeed = _GenProjectile->MoveSpeed;
		_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());
	}
	
}

void AMonsterGeneralCharacter::EndAttack_FromNotify()
{
	SetMonsterState(EMonsterState::E_Idle);
}

void AMonsterGeneralCharacter::TrySetHitReaction()
{
	SetMonsterState(EMonsterState::E_HitReact);
}

void AMonsterGeneralCharacter::EndHitReact_FromNotify()
{
	SetMonsterState(EMonsterState::E_Idle);
}

void AMonsterGeneralCharacter::EndDying_FromNotify()
{
	LOGTEXT_LOG(TEXT("몬스터 죽음: %s"), *this->GetName());
	
	Destroy();
}

