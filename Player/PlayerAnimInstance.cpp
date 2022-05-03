// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "PlayableCharacter.h"
#include "PortFolio.h"
#include "GameFramework/PawnMovementComponent.h"


UPlayerAnimInstance::UPlayerAnimInstance()
{
	SetRootMotionMode(ERootMotionMode::Type::IgnoreRootMotion);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 자동으로 붙어있는 폰의 데이터 가져오기
	if(PlayerCharacter != nullptr)
	{
		// 현재 Playable Character 클래스 가져오기
		// 이동속도, 방향
		MoveSpeed = PlayerCharacter->GetVelocity().Size();
		MoveForwardValue = PlayerCharacter->GetCurrentMoveForward();
		MoveRightValue = PlayerCharacter->GetCurrentMoveRight();
		// 점프중
		bOnJumping = PlayerCharacter->GetMovementComponent()->IsFalling();
		// 플레이어 현재 정보
		//PlayerState = PlayerCharacter->GetCurrentState();
		// 행동 정보
		//PlayerActingType = PlayerCharacter->GetCurrentActingType();
	}
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	auto OwnerPawn = TryGetPawnOwner();
	if(OwnerPawn)
	{
		PlayerCharacter = Cast<APlayableCharacter>(OwnerPawn);
	}
}

// Acting(공격, 구르기 등) 재생하는 함수
void UPlayerAnimInstance::PlayActingAnimation(EActingType NextActingType)
{
	if(NextActingType == EActingType::E_None)	return;

	if(NextActingType == EActingType::E_AttackBasic)
	{
		PlayerCharacter->SetCurrentActingType(EActingType::E_AttackBasic);
	}

	else if(NextActingType == EActingType::E_AttackSkill1)
	{
		PlayerCharacter->SetCurrentActingType(EActingType::E_AttackSkill1);
	}
}


// ///////////////////////////////////////////////////////////////////////////
// 자동 호출 노티파이 함수
#pragma region 노티파이 모음
void UPlayerAnimInstance::AnimNotify_CanAttackCombo()
{
	if(PlayerCharacter != nullptr)
	{
		PlayerCharacter->bCanBasicAttackCombo = true;
		PlayerCharacter->bDontEndBasicAttack = false;
	}
		
}

void UPlayerAnimInstance::AnimNotify_EndJump()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->EndJumpFromNotify();
}

void UPlayerAnimInstance::AnimNotify_EndAttackBasic()
{
	//BasicAttackNum++;
	if(PlayerCharacter != nullptr)
	{
		PlayerCharacter->EndActingFromNotify();
		PlayerCharacter->bCanBasicAttackCombo = false;
	}
}

void UPlayerAnimInstance::AnimNotify_EndDodge()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->EndActingFromNotify();
}

void UPlayerAnimInstance::AnimNotify_EndSkill1()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->EndActingFromNotify();
}

void UPlayerAnimInstance::AnimNotify_EndSkill2()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->EndActingFromNotify();
}

void UPlayerAnimInstance::AnimNotify_EndSkill3()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->EndActingFromNotify();
}

void UPlayerAnimInstance::AnimNotify_TriggerAttackBasic()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->ActionTrigger_AttackBasic();
}

void UPlayerAnimInstance::AnimNotify_TriggerDodge()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->ActionTrigger_Dodge();
}

void UPlayerAnimInstance::AnimNotify_TriggerSkill1()
{
	if(PlayerCharacter != nullptr)
    	PlayerCharacter->ActionTrigger_Skill1();
}

void UPlayerAnimInstance::AnimNotify_TriggerSkill2()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->ActionTrigger_Skill2();
}

void UPlayerAnimInstance::AnimNotify_TriggerSkill3()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->ActionTrigger_Skill3();
}

void UPlayerAnimInstance::AnimNotify_Misc1()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->MiscTrigger1();
}

void UPlayerAnimInstance::AnimNotify_Misc2()
{
	if(PlayerCharacter != nullptr)
		PlayerCharacter->MiscTrigger2();
}
#pragma endregion 


