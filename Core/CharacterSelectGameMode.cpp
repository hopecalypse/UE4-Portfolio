// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CharacterSelectGameMode.h"

#include "GameManagerInstance.h"
#include "PortFolio.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SelectCharacter/CharacterSelectController.h"
#include "SelectCharacter/SelectCharacter.h"
#include "UI/SelectCharacterWidget.h"

ACharacterSelectGameMode::ACharacterSelectGameMode()
{
	// 위젯 블루프린트 찾기
	static ConstructorHelpers::FClassFinder<UUserWidget> _SelCharWidgetClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_SelectCharacterWidget.WBP_SelectCharacterWidget_C'"));
	if(_SelCharWidgetClass.Succeeded())
		SelectCharacterWidgetClass = _SelCharWidgetClass.Class;
	
}

void ACharacterSelectGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 컨트롤러 가져오기
	SelectController = Cast<ACharacterSelectController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	// 캐릭터 선택 위젯 생성
	SelectCharacterWidget = Cast<USelectCharacterWidget>(CreateWidget(GetWorld(), SelectCharacterWidgetClass));
	SelectCharacterWidget->AddToViewport();
}

void ACharacterSelectGameMode::Select_FromCharacter(ASelectCharacter* _SelectCharacter)
{
	// 만약 캐릭터 선택 중에 들어오면 무시
	if(CurrentSelectCharacter != nullptr)
		return;
	
	// 캐릭터 선택
	CurrentSelectCharacter = _SelectCharacter;
	CurrentSelectCharacter->bIsSelecting = true;

	// 카메라 이동
	SelectController->SetViewTargetWithBlend(CurrentSelectCharacter, 1.f);

	// 정보, 선택 위젯 띄우기
	SelectCharacterWidget->OpenCharacterPopup(_SelectCharacter->GetSelectClass());
}

void ACharacterSelectGameMode::ExitSelect_FromButton()
{
	LOGTEXT_LOG(TEXT("캐릭터 선택 취소"));

	// 카메라 돌아가기
	SelectController->SetViewTargetWithBlend(SelectController->DefaultPawn, 1.f);

	// 선택 취소
	CurrentSelectCharacter->bIsSelecting = false;
	CurrentSelectCharacter = nullptr;
	
}

void ACharacterSelectGameMode::ConfirmSelect_FromButton()
{
	LOGTEXT_LOG(TEXT("캐릭터 선택 확정"));

	// GI에 선택 클래스 정보 전달
	Cast<UGameManagerInstance>(GetGameInstance())->SelectPlayerClass(CurrentSelectCharacter->PlayerClass);
	
	// 현재 선택중인 클래스: CurrentSelectCharacter->PlayerClass
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Dungeon"));

	// 입력 모드 변경하기
	FInputModeGameOnly _GameInputMode;
	SelectController->SetInputMode(_GameInputMode);
	SelectController->bShowMouseCursor = false;
}
