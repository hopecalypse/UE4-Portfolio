// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

struct FClassActingInfo;
class UImage;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UProgressBar* HpProgressBar;
	UProgressBar* MpProgressBar;
	UTextBlock* HpBarText;
	UTextBlock* MpBarText;

	UImage* FaceImage;

	void GetWidgetUIObject();

	// HP바 관련
public:
	void SyncHpMpBar(float _CurHp, float _MaxHp, float _CurMp, float _MaxMp);

	// Acting 이미지 세팅
public:
	void SetHUDSkillImages(const FClassActingInfo _ActingInfo);

	// Acting 관련(쿨타임)
private:
	UImage* Image_BasicAttack;
	UProgressBar* CoolDown_BasicAttack;
	UImage* Image_Skill1;
	UProgressBar* CoolDown_Skill1;
	UImage* Image_Skill2;
	UProgressBar* CoolDown_Skill2;
	UImage* Image_Skill3;
	UProgressBar* CoolDown_Skill3;

	bool bCooling_BasicAttack;
	bool bCooling_Skill1;
	bool bCooling_Skill2;
	bool bCooling_Skill3;

	float CoolingTime_BasicAttack;
	float CoolingValue_BasicAttack;
	float CoolingTime_Skill1;
	float CoolingValue_Skill1;
	float CoolingTime_Skill2;
    float CoolingValue_Skill2;
	float CoolingTime_Skill3;
	float CoolingValue_Skill3;

public:
	// 쿨다운 표시 시작
	void StartCooldown_BasicAttack(float _Cooldown);
	void StartCoolDown_Skill1(float _Cooldown);
	void StartCoolDown_Skill2(float _Cooldown);
	void StartCoolDown_Skill3(float _Cooldown);

	// 쿨다운 종료 시
	void EndCooldown_BasicAttack();
	void EndCooldown_Skill1();
	void EndCooldown_Skill2();
	void EndCooldown_Skill3();
	
};
