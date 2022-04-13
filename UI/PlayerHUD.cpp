// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUD.h"

#include "PortFolio.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/CustomStructs.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	GetWidgetUIObject();

	// Face 이미지 위젯에 머티리얼 할당하기(배경을 투명하게 만들기 위해)
	UMaterialInstance* _Material = LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/_Textures/HUD/FaceDepthClear_Instance.FaceDepthClear_Instance'"));
	UMaterialInstanceDynamic* _MaterialDynamic = UMaterialInstanceDynamic::Create(_Material, nullptr);
	UTexture* _FaceTexture = LoadObject<UTexture>(nullptr, TEXT("TextureRenderTarget2D'/Game/_Textures/HUD/HUDPlayerFace.HUDPlayerFace'"));
	_MaterialDynamic->SetTextureParameterValue(TEXT("OutputTexture"), _FaceTexture);

	// 페이스 HUD 설정
	FaceImage = Cast<UImage>(GetWidgetFromName(TEXT("FaceImage")));
	FaceImage->SetBrushFromMaterial(_MaterialDynamic);

	// Acting 위젯 찾기
	Image_BasicAttack = Cast<UImage>(GetWidgetFromName(TEXT("Image_BasicAttack")));
	Image_Skill1 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Skill1")));
	Image_Skill2 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Skill2")));
	Image_Skill3 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Skill3")));
	CoolDown_BasicAttack = Cast<UProgressBar>(GetWidgetFromName((TEXT("CoolDown_BasicAttack"))));
	CoolDown_Skill1 = Cast<UProgressBar>(GetWidgetFromName((TEXT("CoolDown_Skill1"))));
	CoolDown_Skill2 = Cast<UProgressBar>(GetWidgetFromName((TEXT("CoolDown_Skill2"))));
	CoolDown_Skill3 = Cast<UProgressBar>(GetWidgetFromName((TEXT("CoolDown_Skill3"))));
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// 기본공격 쿨타임
	if(bCooling_BasicAttack)
	{
		CoolingValue_BasicAttack = FMath::FInterpConstantTo(CoolingValue_BasicAttack, 0, InDeltaTime, 1);
		CoolDown_BasicAttack->SetPercent(CoolingValue_BasicAttack / CoolingTime_BasicAttack);
	}

	// 스킬1 쿨타임
	if(bCooling_Skill1)
	{
		CoolingValue_Skill1 = FMath::FInterpConstantTo(CoolingValue_Skill1, 0, InDeltaTime, 1);
		CoolDown_Skill1->SetPercent(CoolingValue_Skill1 / CoolingTime_Skill1);
	}

	// 스킬2 쿨타임
	if(bCooling_Skill2)
	{
		CoolingValue_Skill2 = FMath::FInterpConstantTo(CoolingValue_Skill2, 0, InDeltaTime, 1);
		CoolDown_Skill2->SetPercent(CoolingValue_Skill2 / CoolingTime_Skill2);
	}

	// 스킬3 쿨타임
	if(bCooling_Skill3)
	{
		CoolingValue_Skill3 = FMath::FInterpConstantTo(CoolingValue_Skill3, 0, InDeltaTime, 1);
		CoolDown_Skill3->SetPercent(CoolingValue_Skill3 / CoolingTime_Skill3);
	}
}

void UPlayerHUD::GetWidgetUIObject()
{
	if(HpProgressBar == nullptr)
		HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HpBar")));
	if(MpProgressBar == nullptr)
		MpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("MpBar")));
	if(HpBarText == nullptr)
		HpBarText = Cast<UTextBlock>(GetWidgetFromName(TEXT("HpBarText")));
	if(MpBarText == nullptr)
		MpBarText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MpBarText")));
}

void UPlayerHUD::SyncHpMpBar(float _CurHp, float _MaxHp, float _CurMp, float _MaxMp)
{
	float _HpPer = _CurHp / _MaxHp;
	float _MpPer = _CurMp / _MaxMp;
	int _CurHpInt = FMath::FloorToInt(_CurHp);
	int _MaxHpInt = FMath::FloorToInt(_MaxHp);
	int _CurMpInt = FMath::FloorToInt(_CurMp);
	int _MaxMpInt = FMath::FloorToInt(_MaxMp);
	
	if(HpProgressBar == nullptr || MpProgressBar == nullptr || HpBarText == nullptr || MpBarText == nullptr)
		GetWidgetUIObject();
	
	HpProgressBar->SetPercent(_HpPer);
	MpProgressBar->SetPercent(_MpPer);
	HpBarText->SetText(FText::FromString(FString::FromInt(_CurHpInt) + TEXT(" / ") + FString::FromInt(_MaxHpInt)));
	MpBarText->SetText(FText::FromString(FString::FromInt(_CurMpInt) + TEXT(" / ") + FString::FromInt(_MaxMpInt)));
}


void UPlayerHUD::SetHUDSkillImages(const FClassActingInfo _ActingInfo)
{
	// 스킬 이미지들 업데이트
	if(_ActingInfo.BasicAttackImage != nullptr)
		Image_BasicAttack->SetBrushFromTexture(_ActingInfo.BasicAttackImage);
	if(_ActingInfo.Skill1Image != nullptr)
		Image_Skill1->SetBrushFromTexture(_ActingInfo.Skill1Image);
	if(_ActingInfo.Skill2Image != nullptr)
		Image_Skill2->SetBrushFromTexture(_ActingInfo.Skill2Image);
	if(_ActingInfo.Skill3Image != nullptr)
		Image_Skill3->SetBrushFromTexture(_ActingInfo.Skill3Image);

	// 만약 숨겨져 있다면 보이게 하기
	if(GetVisibility() == ESlateVisibility::Hidden)
		SetVisibility(ESlateVisibility::Visible);
}


void UPlayerHUD::StartCooldown_BasicAttack(float _Cooldown)
{
	// Progress 설정
	CoolDown_BasicAttack->SetPercent(0.f);
	CoolDown_BasicAttack->SetVisibility(ESlateVisibility::Visible);
	// 쿨다운 시작
	CoolingValue_BasicAttack = _Cooldown;
	CoolingTime_BasicAttack = _Cooldown;
	bCooling_BasicAttack = true;
}

void UPlayerHUD::StartCoolDown_Skill1(float _Cooldown)
{
	// Progress 설정
	CoolDown_Skill1->SetPercent(0.f);
	CoolDown_Skill1->SetVisibility(ESlateVisibility::Visible);
	// 쿨다운 시작
	CoolingValue_Skill1 = _Cooldown;
	CoolingTime_Skill1 = _Cooldown;
	bCooling_Skill1 = true;
}

void UPlayerHUD::StartCoolDown_Skill2(float _Cooldown)
{
	// Progress 설정
	CoolDown_Skill2->SetPercent(0.f);
	CoolDown_Skill2->SetVisibility(ESlateVisibility::Visible);
	// 쿨다운 시작
	CoolingValue_Skill2 = _Cooldown;
	CoolingTime_Skill2 = _Cooldown;
	bCooling_Skill2 = true;
}

void UPlayerHUD::StartCoolDown_Skill3(float _Cooldown)
{
	// Progress 설정
	CoolDown_Skill3->SetPercent(0.f);
	CoolDown_Skill3->SetVisibility(ESlateVisibility::Visible);
	// 쿨다운 시작
	CoolingValue_Skill3 = _Cooldown;
	CoolingTime_Skill3 = _Cooldown;
	bCooling_Skill3 = true;
}

void UPlayerHUD::EndCooldown_BasicAttack()
{
	CoolDown_BasicAttack->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::EndCooldown_Skill1()
{
	CoolDown_Skill1->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::EndCooldown_Skill2()
{
	CoolDown_Skill2->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::EndCooldown_Skill3()
{
	CoolDown_Skill3->SetVisibility(ESlateVisibility::Hidden);
}




