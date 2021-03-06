// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"

#include "DrawDebugHelpers.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "PlayerAnimInstance.h"
#include "PortFolio.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Combat/ProjectileGeneral.h"
#include "Components/CanvasPanel.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"
#include "Core/AudioDataAsset.h"
#include "Core/GameManagerInstance.h"
#include "Core/PortFolioGameModeBase.h"
#include "Dungeon/DungeonManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/MonsterGeneralCharacter.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Manager/SoundManager.h"
#include "UI/PlayerHUD.h"

// Sets default values
APlayableCharacter::APlayableCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 클래스 찾기
	static ConstructorHelpers::FClassFinder<AActor> _DecalBPClass(TEXT("Blueprint'/Game/_Blueprints/Combat/BP_PlayerCastingDecal.BP_PlayerCastingDecal_C'"));
	if(_DecalBPClass.Succeeded())
		ActingInfos.CastingDecalActorClass = _DecalBPClass.Class;
	static ConstructorHelpers::FClassFinder<AActor> _LvlUpClass(TEXT("Blueprint'/Game/_Blueprints/Player/BP_LevelUpParticle.BP_LevelUpParticle_C'"));
	if(_LvlUpClass.Succeeded())
		LevelUpAClass = _LvlUpClass.Class;
	// 오브젝트 찾기
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> _MinimapRT(TEXT("TextureRenderTarget2D'/Game/_Textures/Minimap/MinimapRenderTarget.MinimapRenderTarget'"));
	if(_MinimapRT.Succeeded())
		MinimapRenderTarget = _MinimapRT.Object;
	static ConstructorHelpers::FObjectFinder<UPaperSprite> _PlayerSprite(TEXT("PaperSprite'/Game/_Textures/Minimap/Player.Player'"));
	if(_PlayerSprite.Succeeded())
		PlayerSprite = _PlayerSprite.Object;

	// 필요 컴포넌트 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	FaceCaptureCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Face Capture"));

	MinimapArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Minimap Spring Arm"));
	MinimapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap SceneCapture"));
	MinimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Minimap Sprite"));
	
	// 컴포넌트 설정
	MinimapArm->SetupAttachment(RootComponent);
	MinimapSceneCapture->SetupAttachment(MinimapArm);
	MinimapSprite->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->TargetOffset = FVector(0.f, 0.f, 150.f);
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerCharacter"));
	FaceCaptureCamera->SetupAttachment(GetMesh());
	FaceCaptureCamera->AttachTo(GetMesh(), FName(TEXT("FaceCaptureSocket")), EAttachLocation::KeepRelativeOffset);
	FaceCaptureCamera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	FaceCaptureCamera->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;

	MinimapArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	MinimapArm->TargetArmLength = 31000.f;
	MinimapSceneCapture->TextureTarget = MinimapRenderTarget;
	MinimapSceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapSceneCapture->OrthoWidth = 14000.f;
	MinimapSceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	MinimapSprite->SetSprite(PlayerSprite);
	MinimapSprite->SetRelativeScale3D(FVector(1.6f, 2.f, 2.f));
	MinimapSprite->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	MinimapSprite->SetRelativeLocation(FVector(0.f, 0.f, 30000.f));
	MinimapSprite->SetSpriteColor(FLinearColor::Blue);

	// State 초기화
	SetCurrentState(EPlayerState::E_Idle);
}

void APlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Anim Instance 찾아오기(각 자식 클래스의 생성자에서 메시, 애니메이션을 찾아온 이후)
	AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 플레이어 HUD 위젯 초기화
	if(PlayerHUD == nullptr)
		PlayerHUD = UDungeonManager::Instance()->PlayerHUD;
	if(PlayerHUD == nullptr)
		PlayerHUD = Cast<APortFolioGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->GetPlayerHUDWidget();
	// 처음 캐릭터 스탯 가져오기
	PlayerInfo.Level = 1;
	SetPlayerStat(1);
	// HP, MP바 위젯 초기화
	PlayerHUD->SyncHpMpBar(PlayerInfo.CurrentHp, PlayerInfo.MaxHp, PlayerInfo.CurrentMp, PlayerInfo.MaxMp);
	// Exp바 초기화
	PlayerHUD->SyncExpBar(PlayerInfo.CurrentExp, PlayerInfo.NextExp);
	// 스킬 이미지
	PlayerHUD->SetHUDSkillImages(ActingInfos);
	
	// Face 캡쳐 소스 설정
	FaceCaptureCamera->ShowOnlyComponents.Add(GetMesh());
}

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickUpdateStatus();
	//UpdateDungeonLocation();
}

void APlayableCharacter::GetExp(int32 _Exp)
{
	PlayerInfo.CurrentExp += _Exp;
	PlayerHUD->SyncExpBar(PlayerInfo.CurrentExp, PlayerInfo.NextExp);

	LOGTEXT_LOG(TEXT("경험치 획득:%d"), _Exp);
	
	// 레벨업?
	if(PlayerInfo.CurrentExp >= PlayerInfo.NextExp)
		LevelUp();
}

void APlayableCharacter::LevelUp()
{
	if(PlayerInfo.Level == 3)
		return;
	
	LOGTEXT_ERROR(TEXT("레벨 업!"));
	FActorSpawnParameters _SpawnParams;
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(LevelUpAClass, GetActorLocation(), FRotator::ZeroRotator, _SpawnParams);
	
	PlayerInfo.Level++;
	SetPlayerStat(PlayerInfo.Level);

	PlayerHUD->SyncExpBar(PlayerInfo.CurrentExp, PlayerInfo.NextExp);
	
	// 효과음 재생
	UGameplayStatics::PlaySound2D(GetWorld(), USoundManager::Instance()->Data->LevelUp);
}

void APlayableCharacter::SetCurrentState(EPlayerState NextState)
{
    if(CurrentState == NextState) return;
	if(CurrentState == EPlayerState::E_Dying)	return;
	
	CurrentState = NextState;
}

void APlayableCharacter::SetCurrentActingType(EActingType NextAct)
{
	if(CurrentActingType == NextAct)	return;
	
	CurrentActingType = NextAct;
}

// 애니메이션 관련 업데이트
void APlayableCharacter::TickUpdateStatus()
{
	//점프는 AnimNotify로 변경
	// if(GetMovementComponent()->IsFalling())
	// 	CurrentStatus = EPlayerStatus::E_Jumping;
	if(GetCurrentState() == EPlayerState::E_Idle || GetCurrentState() == EPlayerState::E_Moving)
	{
		if(CurrentMoveForwardValue > 0 || CurrentMoveRightValue > 0)
			SetCurrentState(EPlayerState::E_Moving);

		else if(CurrentMoveForwardValue == 0 && CurrentMoveRightValue == 0)
			SetCurrentState(EPlayerState::E_Idle);
	}
}

void APlayableCharacter::ChangeMp(float _ManaPoint)
{
	LOGTEXT_LOG(TEXT("플레이어 마나 변화"));

	PlayerInfo.CurrentMp -= _ManaPoint;
	PlayerHUD->SyncHpMpBar(PlayerInfo.CurrentHp, PlayerInfo.MaxHp, PlayerInfo.CurrentMp, PlayerInfo.MaxMp);
}

void APlayableCharacter::UpdateDungeonLocation()
{
	UDungeonManager::Instance()->UpdatePlayerLocation(this);
}

void APlayableCharacter::CheatLevelUp()
{
	LevelUp();
}

#pragma region 기본 조작 관련(이동, 점프)

// 조작 관련
// Called to bind functionality to input
void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayableCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayableCharacter::MoveRight);
	PlayerInputComponent->BindAxis("RotateYaw", this, &APlayableCharacter::RotateYaw);
	PlayerInputComponent->BindAxis("RotatePitch", this, &APlayableCharacter::RotatePitch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayableCharacter::StartJump);
	
	PlayerInputComponent->BindAction("Button1", IE_Pressed, this, &APlayableCharacter::GetActionInput_AttackBasic);
	PlayerInputComponent->BindAction("Button3", IE_Pressed, this, &APlayableCharacter::GetActionInput_Dodge);
	PlayerInputComponent->BindAction("Button4", IE_Pressed, this, &APlayableCharacter::GetActionInput_Skill1);
	PlayerInputComponent->BindAction("Button5", IE_Pressed, this, &APlayableCharacter::GetActionInput_Skill2);
	PlayerInputComponent->BindAction("Button6", IE_Pressed, this, &APlayableCharacter::GetActionInput_Skill3);

	PlayerInputComponent->BindAction(TEXT("Tab"), IE_Pressed, this, &APlayableCharacter::SwitchCharacter);
	PlayerInputComponent->BindAction(TEXT("LevelUp"), IE_Pressed, this, &APlayableCharacter::CheatLevelUp);
}

void APlayableCharacter::MoveForward(float AxisValue)
{
	//이동 불가
	if(GetCurrentState() == EPlayerState::E_ActingStatic)	return;
	
	// 애니메이션을 위한 MoveValue 변수
	CurrentMoveForwardValue = AxisValue;
	
	// 반영될 변수들
	float Multiplier;
	// 점프중이면 속도 10분의 1로 줄이기
	if(GetCurrentState() == EPlayerState::E_Jumping)
		Multiplier = 0.2f;
	else if(GetCurrentState() == EPlayerState::E_ActingMoving)
		Multiplier = 0.6f;
	else
		Multiplier = 1.f;
	
	//최종 속도
	float MoveValue = AxisValue * Multiplier;
	AddMovementInput(GetActorForwardVector(), MoveValue);
}

void APlayableCharacter::MoveRight(float AxisValue)
{
	//이동 불가
	if(GetCurrentState() == EPlayerState::E_ActingStatic)	return;
	
	// 애니메이션을 위한 MoveValue 변수
	CurrentMoveRightValue = AxisValue;

	// 반영될 변수들
	float Multiplier;
	// 점프중이면 속도 10분의 1로 줄이기
	if(GetCurrentState() == EPlayerState::E_Jumping)
		Multiplier = 0.2f;
	else if(GetCurrentState() == EPlayerState::E_ActingMoving)
		Multiplier = 0.6f;
	else
		Multiplier = 1.f;

	//최종 속도
	float MoveValue = AxisValue * Multiplier;
	AddMovementInput(GetActorRightVector(), MoveValue);
}

void APlayableCharacter::RotateYaw(float AxisValue)
{
	if(bCastingIgnoreRot)
		return;
	
	float RotateValue = AxisValue;
	AddControllerYawInput(RotateValue);
}

void APlayableCharacter::RotatePitch(float AxisValue)
{
	float RotateValue = AxisValue;
	AddControllerPitchInput(RotateValue);
}

void APlayableCharacter::StartJump()
{
	// 이미 점프중이 아닐 때만
	if(GetCurrentState() == EPlayerState::E_Idle || GetCurrentState() == EPlayerState::E_Moving)
	{
		Super::Jump();
		SetCurrentState(EPlayerState::E_Jumping);
		// ~점프 완료시 AnimNotify 호출됨(Idle로)
	}
}

void APlayableCharacter::SwitchCharacter()
{
	LOGTEXT_LOG(TEXT("캐릭터 전환버튼 클릭"));

	if(GetCurrentState() == EPlayerState::E_Idle)
	{
		UGameManagerInstance* _GameInstance = Cast<UGameManagerInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		_GameInstance->SwitchCharacter(GetClass(), this);
	}
}

#pragma endregion 

#pragma region 조작 관련(Acting 입력)

// 공격 입력 받기 함수
void APlayableCharacter::GetActionInput_AttackBasic()
{
	// 확인: State로 가능한지 판별(Idle, Move일때만) or Attack Combo 가능할 때
	
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

void APlayableCharacter::GetActionInput_Dodge()
{
	// 확인: State로 가능한지 판별(Idle, Move일때만)
    if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)	return;
	// 확인: 쿨타임 중인지
	
	
	// 실행~
	// 상태State 설정
	SetCurrentState(EPlayerState::E_ActingStatic);
	// Acting Type 설정
	SetCurrentActingType(EActingType::E_Dodge);
	// 애니메이션 재생
	AnimInstance->PlayActingAnimation(EActingType::E_Dodge);
	// 무적 상태 설정
	SetIsInvincible(true);
}

void APlayableCharacter::GetActionInput_Skill1()
{
	// 확인: State로 가능한지 판별(Idle, Move일때만)
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)	return;
	// 확인: 쿨타임 중인지
	if(bSkill1Cooling)		return;
	// 확인: 마나
	if(PlayerInfo.CurrentMp < ActingInfos.Skill1MP)
		return;
	ChangeMp(ActingInfos.Skill1MP);

	// 실행~
	// 공격 유형에 따라 이동 가능 여부 판별
	EPlayerState _ActingState = ActingInfos.Skill1AttackMovable ? EPlayerState::E_ActingMoving : EPlayerState::E_ActingStatic;
	// 상태State 설정
	SetCurrentState(_ActingState);
	// Acting Type 설정
	SetCurrentActingType(EActingType::E_AttackSkill1);
	// 애니메이션 재생
	AnimInstance->PlayActingAnimation(EActingType::E_AttackSkill1);
	// 쿨다운 설정
	Cooldown_Skill1();
}

void APlayableCharacter::GetActionInput_Skill2()
{
	if(ActingInfos.bSKill2Locked)
		return;
	// 확인: State로 가능한지 판별(Idle, Move일때만)
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)	return;
	// 확인: 쿨타임 중인지
	if(bSkill2Cooling)	return;

	if(PlayerInfo.CurrentMp < ActingInfos.Skill2MP)
		return;
	ChangeMp(ActingInfos.Skill2MP);

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

void APlayableCharacter::GetActionInput_Skill3()
{
	if(ActingInfos.bSkill3Locked)
		return;
	// 확인: State로 가능한지 판별(Idle, Move일때만)
	if(GetCurrentState() != EPlayerState::E_Idle && GetCurrentState() != EPlayerState::E_Moving)	return;
	// 확인: 쿨타임 중인지
	if(bSkill3Cooling)		return;

	if(PlayerInfo.CurrentMp < ActingInfos.Skill3MP)
		return;
	ChangeMp(ActingInfos.Skill3MP);
	// 실행~
	// 공격 유형에 따라 이동 가능 여부 판별
	EPlayerState _ActingState = ActingInfos.Skill3AttackMovable ? EPlayerState::E_ActingMoving : EPlayerState::E_ActingStatic;
	// 상태State 설정
	SetCurrentState(_ActingState);
	// Acting Type 설정
	SetCurrentActingType(EActingType::E_AttackSkill3);
	// 애니메이션 재생
	AnimInstance->PlayActingAnimation(EActingType::E_AttackSkill3);
	// 쿨다운 설정
	Cooldown_Skill3();
}

#pragma endregion

#pragma region 쿨다운 함수들

void APlayableCharacter::Cooldown_AttackBasic()
{
	bBasicAttackCooling = true;

	// 타이머 설정
	float _Cooldown = ActingInfos.BasicAttackCooldown;
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, FName("OnCooldownEnd_AttackBasic"));
	GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, _Cooldown, false);

	// HUD 표시 함수
	PlayerHUD->StartCooldown_BasicAttack(_Cooldown);
}

void APlayableCharacter::Cooldown_Skill1()
{
	bSkill1Cooling = true;
	
	// 타이머 설정
	float _Cooldown = ActingInfos.Skill1Cooldown;
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, FName("OnCooldownEnd_Skill1"));
	GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, _Cooldown, false);

	// HUD 표시 함수
	PlayerHUD->StartCoolDown_Skill1(_Cooldown);
}

void APlayableCharacter::Cooldown_Skill2()
{
	bSkill2Cooling = true;

	// 타이머 설정
	float _Cooldown = ActingInfos.Skill2Cooldown;
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, FName("OnCooldownEnd_Skill2"));
	GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, _Cooldown, false);

	// HUD 표시 함수
	PlayerHUD->StartCoolDown_Skill2(_Cooldown);
}

void APlayableCharacter::Cooldown_Skill3()
{
	bSkill3Cooling = true;

	// 타이머 설정
	float _Cooldown = ActingInfos.Skill3Cooldown;
	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, FName("OnCooldownEnd_Skill3"));
	GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, _Cooldown, false);

	// HUD 표시 함수
	PlayerHUD->StartCoolDown_Skill3(_Cooldown);
}

// 쿨다운 종료 시 타이머 호출
void APlayableCharacter::OnCooldownEnd_AttackBasic()
{
	bBasicAttackCooling = false;
}

void APlayableCharacter::OnCooldownEnd_Skill1()
{
	bSkill1Cooling = false;
}

void APlayableCharacter::OnCooldownEnd_Skill2()
{
	bSkill2Cooling = false;
}

void APlayableCharacter::OnCooldownEnd_Skill3()
{
	bSkill3Cooling = false;
}

#pragma endregion 

#pragma region 공격 관련(Acting 실행)
void APlayableCharacter::ActionTrigger_AttackBasic()
{
	if(ActingInfos.BasicAttackProjectile != nullptr)
		ShootMovingProjectile(ActingInfos.BasicAttackProjectile, ActingInfos.BasicAttackProjectileLifeTime, ActingInfos.bBaiscAttackDamageAtProjectile, ActingInfos.BasicAttackPower);
	if(ActingInfos.IsBasicAttackMeleeSweep)
		MeleeSweepAttack(ActingInfos.BasicAttackMeleeRange, ActingInfos.BasicAttackPower);
	
}

void APlayableCharacter::ActionTrigger_Dodge()
{
}

void APlayableCharacter::ActionTrigger_Skill1()
{
	if(ActingInfos.Skill1Projectile != nullptr)
		ShootMovingProjectile(ActingInfos.Skill1Projectile, ActingInfos.Skill1ProjectileLifeTime, ActingInfos.bSkill1DamageAtProjectile, ActingInfos.Skill1AttackPower);
	if(ActingInfos.IsSkill2MeleeSweep)
		MeleeSweepAttack(ActingInfos.Skill1MeleeRange, ActingInfos.Skill1AttackPower);
}

void APlayableCharacter::ActionTrigger_Skill2()
{
	if(ActingInfos.Skill2Projectile != nullptr)
		ShootMovingProjectile(ActingInfos.Skill2Projectile, ActingInfos.Skill2ProjectileLifeTime, ActingInfos.bSkill2DamageAtProjectile, ActingInfos.Skill2AttackPower);
	if(ActingInfos.IsSkill2MeleeSweep)
		MeleeSweepAttack(ActingInfos.Skill2MeleeRange, ActingInfos.Skill2AttackPower);
}

void APlayableCharacter::ActionTrigger_Skill3()
{
	if(ActingInfos.Skill3Projectile != nullptr)
		ShootMovingProjectile(ActingInfos.Skill3Projectile, ActingInfos.Skill3ProjectileLifeTime, ActingInfos.bSkill3DamageAtProjectile, ActingInfos.Skill3AttackPower);
    if(ActingInfos.IsSkill3MeleeSweep)
    	MeleeSweepAttack(ActingInfos.Skill3MeleeRange, ActingInfos.Skill3AttackPower);
}

void APlayableCharacter::MiscTrigger1()
{
	
}

void APlayableCharacter::MiscTrigger2()
{
	
}

void APlayableCharacter::MeleeSweepAttack(float _AttackRange, float _AttackPower, float _AttackRadius)
{
	// 공격 성공 Check
	FHitResult _HitResult;
	FCollisionQueryParams _Params(NAME_None, false, this);
	bool _bResult = GetWorld()->SweepSingleByChannel(OUT _HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * _AttackRange,
		FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel5, FCollisionShape::MakeSphere(_AttackRadius), _Params);

	// 몬스터 타격 성공했을 때
	if(_bResult)
	{
		if(Cast<AMonsterGeneralCharacter>(_HitResult.Actor) != nullptr)
		{
			Cast<AMonsterGeneralCharacter>(_HitResult.Actor)->ChangeMonsterHp(_AttackPower);
			// Hit React 설정 시도
			Cast<AMonsterGeneralCharacter>(_HitResult.Actor)->TrySetHitReaction();
		}
		LOGTEXT_LOG(TEXT("Sweep 공격 충돌 판정: %s"), *_HitResult.Actor->GetName());
	}

	// 디버그 그리기
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

void APlayableCharacter::ShootMovingProjectile(TSubclassOf<AProjectileGeneral> _TargetProjectile, float _LifeTime, bool _bAttackDMG, float _AttackDamage, float _StartPosition)
{
	// BP 액터 생성 Param
	FVector _ForwardVector = GetActorForwardVector();
	FActorSpawnParameters _SpawnParams = {};
	_SpawnParams.OverrideLevel = GetLevel();
	_SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_SpawnParams.bDeferConstruction = true;
	AProjectileGeneral* _GenProjectile = GetWorld()->SpawnActor<AProjectileGeneral>(_TargetProjectile, GetActorLocation() + _ForwardVector * _StartPosition, _ForwardVector.Rotation(), _SpawnParams);
	
	// 발사체 이동 설정
	_GenProjectile->SetLifeTime(_LifeTime);
	if(_bAttackDMG)
		_GenProjectile->SetAttackDamage(_AttackDamage);
	// 기본은 캐릭터 정면 방향으로 날아가게 설정
	_GenProjectile->GetProjectileMovement()->Velocity = _ForwardVector;
	_GenProjectile->FinishSpawning(_GenProjectile->GetTransform());
}

void APlayableCharacter::GetAttacked(float _AttackDamage)
{
	//LOGTEXT_LOG(TEXT("플레이어 공격받음: %f"), _AttackDamage);
	PlayerInfo.CurrentHp -= _AttackDamage;
	//LOGTEXT_LOG(TEXT("플레이어 체력 변화: %f"), &PlayerInfo.CurrentHp);
	if(PlayerInfo.CurrentHp <= 0)
	{
		PlayerInfo.CurrentHp = 0;
		StartDeath();
	}
	PlayerHUD->SyncHpMpBar(PlayerInfo.CurrentHp, PlayerInfo.MaxHp, PlayerInfo.CurrentMp, PlayerInfo.MaxMp);
}

void APlayableCharacter::StartDeath()
{
	PlayerHUD->SetVisibility(ESlateVisibility::Hidden);
	SetCurrentState(EPlayerState::E_Dying);

	FTimerHandle _TimerHandle;
	FTimerDelegate _TimerDelegate;
	_TimerDelegate.BindUFunction(this, TEXT("GameOver"));
	GetWorld()->GetTimerManager().SetTimer(_TimerHandle, _TimerDelegate, 3.f, false);
}

void APlayableCharacter::GameOver()
{
	UDungeonManager::Instance()->GameOver();
}

#pragma endregion 

#pragma region 애니메이션 노티파이 함수들
////////////////////////////////////////////////////////////////////////////////////////////
/// 노티파이 함수
void APlayableCharacter::EndJumpFromNotify()
{
	// 점프 완료시 Notify(애님인스턴스)에서 호출
	SetCurrentState(EPlayerState::E_Idle);
}

void APlayableCharacter::EndActingFromNotify()
{
	// 행동(공격, 회피 등) 완료시 Notify(애님인스턴스)에서 호출
	// 콤보중일 때
	if(bDontEndBasicAttack)
		return;
	
	bCanBasicAttackCombo = false;
	AnimInstance->BasicAttackNum = 0;
	SetCurrentState(EPlayerState::E_Idle);
	SetCurrentActingType(EActingType::E_None);
	// 무적 상태 종료
	if(GetIsInvincible())	SetIsInvincible(false);
	// Casting 종료, 회전가능
	if(bCastingIgnoreRot)
		bCastingIgnoreRot = false;
}

#pragma endregion

void APlayableCharacter::SetPlayerStat(int16 Level)
{
	UDataTable* _StatTable = Cast<UGameManagerInstance>(GetGameInstance())->ClassStatTable;

	FString _ClassName;
	if(Class == EPlayerClass::E_Warrior)
		_ClassName = TEXT("Warrior");
	else if(Class == EPlayerClass::E_Mage)
		_ClassName = TEXT("Mage");
	else
		_ClassName = TEXT("Gunner");

	LOGTEXT_LOG(TEXT("%s, %s"), *_ClassName, *_StatTable->GetName());
	
	const FClassStat* _FoundStat = _StatTable->FindRow<FClassStat>(FName(_ClassName + FString::FromInt(Level)), TEXT(""));

	// 캐릭터 스탯 할당
	PlayerInfo.MaxHp = _FoundStat->Hp;
	PlayerInfo.CurrentHp = PlayerInfo.MaxHp;
	PlayerInfo.MaxMp = _FoundStat->Mp;
	PlayerInfo.CurrentMp = PlayerInfo.MaxMp;
	PlayerInfo.NextExp = _FoundStat->NextExp;
	
	PlayerInfo.CurrentExp = 0;
	PlayerInfo.Level = Level;
	
	// 스킬 스탯 할당
	ActingInfos.BasicAttackPower = _FoundStat->BasicAttackDamage;
	ActingInfos.Skill1AttackPower = _FoundStat->Skill1Damage;
	ActingInfos.Skill2AttackPower = _FoundStat->Skill2Damage;
	ActingInfos.Skill3AttackPower = _FoundStat->Skill3Damage;

	// 플레이어 HUD 정보 업데이트
	FString _LevelText = TEXT("Level") + FString::FromInt(Level);
	PlayerHUD->LevelText->SetText(FText::FromString(_LevelText));
	PlayerHUD->ClassText->SetText(FText::FromString(_ClassName));
	PlayerHUD->SyncHpMpBar(PlayerInfo.CurrentHp, PlayerInfo.MaxHp, PlayerInfo.CurrentMp, PlayerInfo.MaxMp);

	// 스킬 잠금 상태
	if(Level == 1)
	{
		ActingInfos.bSKill2Locked = true;
		ActingInfos.bSkill3Locked = true;
		GetPlayerHUD()->Locked_Skill2->SetVisibility(ESlateVisibility::Visible);
		GetPlayerHUD()->Locked_Skill3->SetVisibility(ESlateVisibility::Visible);
	}
	else if(Level == 2)
	{
		ActingInfos.bSKill2Locked = false;
		ActingInfos.bSkill3Locked = true;
		GetPlayerHUD()->Locked_Skill2->SetVisibility(ESlateVisibility::Hidden);
		GetPlayerHUD()->Locked_Skill3->SetVisibility(ESlateVisibility::Visible);
	}
	else if(Level == 3)
	{
		ActingInfos.bSKill2Locked = false;
		ActingInfos.bSkill3Locked = false;
		GetPlayerHUD()->Locked_Skill2->SetVisibility(ESlateVisibility::Hidden);
		GetPlayerHUD()->Locked_Skill3->SetVisibility(ESlateVisibility::Hidden);
	}
}
