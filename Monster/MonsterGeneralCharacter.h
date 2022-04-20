// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterAnimInstance.h"
#include "BehaviorTree/BlackboardData.h"
#include "Core/CustomStructs.h"
#include "GameFramework/Character.h"
#include "MonsterGeneralCharacter.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
struct FPathNode;
class UMonsterHpBar;
class UWidgetComponent;
class UBehaviorTree;

struct FMonsterInfo;
enum class EMonsterState : uint8;

UCLASS()
class PORTFOLIO_API AMonsterGeneralCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterGeneralCharacter();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// ! 필요 변수 //
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FMonsterInfo MonsterInfo;			// 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	EMonsterState MonsterState;			// 현재 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FMonsterAttackInfo AttackInfo;		// 공격 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool bIsInvincible;					// 무적 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool bNeverChangableState;			// 상태 변경 가능 여부
	
public:
	UFUNCTION(BlueprintCallable)
	FMonsterInfo GetMonsterInfo() const { return MonsterInfo; };
	UFUNCTION(BlueprintCallable)
	EMonsterState GetMonsterState() const { return MonsterState; };
	UFUNCTION()
	void SetMonsterState(EMonsterState _Value);
	UFUNCTION()
	void TrySetHitReaction();
	UFUNCTION(BlueprintCallable)
	bool GetIsInvincible() const { return bIsInvincible; };
	UFUNCTION(BlueprintCallable)
	void SetIsInvincible(bool _Value) { bIsInvincible = _Value; };

	// ! 몬스터 Stat 관련 필수 함수
	UFUNCTION(BlueprintCallable)
	FMonsterAttackInfo GetMonsterAttackInfo() const { return AttackInfo; };
	UFUNCTION(BlueprintCallable)
	void ChangeMonsterHp(float _HpChangeValue);
	UFUNCTION(BlueprintCallable)
	void DieImmediately();
	

	// ! 필요 컴포넌트 //
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UMonsterAnimInstance* MonsterAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UWidgetComponent* HpbarWidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UMonsterHpBar* HpBarWidgetObject;

public:
	UFUNCTION()
	UMonsterAnimInstance* GetMonsterAnimInstance() { return MonsterAnimInstance; };


	// ! 필요 함수: 애니메이션 //
public:
	void AttackTrigger_FromNotify();
	void EndAttack_FromNotify();
	void EndHitReact_FromNotify();
	void EndDying_FromNotify();


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 필요 변수: AI 
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BlackboardData;

public:
	UBehaviorTree* GetBehaviorTree() { return BehaviorTreeAsset; };
	UBlackboardData* GetBlackboardData() { return BlackboardData; };
protected:
	void SetBehaviorTree(UBehaviorTree* _BT) { BehaviorTreeAsset = _BT; };
	void SetBlackboardData(UBlackboardData* _BlackboardData) { BlackboardData = _BlackboardData; };

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 필요 함수: AI: BT
public:
	UFUNCTION()
	void SetMovementSpeed(float _Speed);

	
	// A* 이동 관련
public:
	TArray<FVector> PathList;
	FVector CurrentPath;
	bool bPathMoving;
	void SetDestination(FVector _Destination);
	void TickMove(float _DeltaTime);

	// 미니맵
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UPaperSpriteComponent* MinimapSprite;
	UPaperSprite* SpriteAsset;


};
