// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimInstance.h"

#include "BossMonsterBase.h"
#include "PortFolio.h"
#include "Core/CustomEnums.h"
#include "Monster/MonsterGeneralCharacter.h"

void UMonsterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// 몬스터 찾기
	auto OwnerPawn = TryGetPawnOwner();
	if(OwnerPawn != nullptr)
	{
		MonsterCharacter = Cast<AMonsterGeneralCharacter>(OwnerPawn);
	}
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 자동으로 몬스터 데이터 가져오기
	if(MonsterCharacter != nullptr)
	{
		 
	}
}

EMonsterState UMonsterAnimInstance::GetMonsterState() const
{
	if(MonsterCharacter == nullptr)
		return EMonsterState::E_Idle;
	
	return MonsterCharacter->GetMonsterState();
}

EMonsterAttackType UMonsterAnimInstance::GetMonsterAttackType() const
{
	if(MonsterCharacter == nullptr)
		return EMonsterAttackType::E_BasicAttack;
	ABossMonsterBase* _Boss = Cast<ABossMonsterBase>(MonsterCharacter);
	if(_Boss == nullptr)
		return EMonsterAttackType::E_BasicAttack;
	return _Boss->AttackType;
}

void UMonsterAnimInstance::AnimNotify_TriggerAttack()
{
	MonsterCharacter->AttackTrigger_FromNotify();
}

void UMonsterAnimInstance::AnimNotify_EndAttack()
{
	MonsterCharacter->EndAttack_FromNotify();
}

void UMonsterAnimInstance::AnimNotify_EndHitReact()
{
	MonsterCharacter->EndHitReact_FromNotify();
}

void UMonsterAnimInstance::AnimNotify_EndDying()
{
	MonsterCharacter->EndDying_FromNotify();
}

void UMonsterAnimInstance::AnimNotify_BossSound()
{
	ABossMonsterBase* _Boss = Cast<ABossMonsterBase>(MonsterCharacter);
	if(_Boss == nullptr)
		_Boss->PlayBossSound_FromNotify();
}


