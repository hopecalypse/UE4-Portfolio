// Copyright Epic Games, Inc. All Rights Reserved.


#include "PortFolioGameModeBase.h"

#include "GameManagerInstance.h"
#include "PortFolio.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerWarrior.h"
#include "UI/PlayerHUD.h"


APortFolioGameModeBase::APortFolioGameModeBase()
{
	// 기본 설정
	DefaultPawnClass = APlayerWarrior::StaticClass();

	static ConstructorHelpers::FClassFinder<UUserWidget> _BPHUDClass(TEXT("WidgetBlueprint'/Game/_Blueprints/UI/WBP_PlayerHUD.WBP_PlayerHUD_C'"));
	if(_BPHUDClass.Succeeded())
		PlayerHUDClass = _BPHUDClass.Class;

	// 플레이어 HUD 생성
	PlayerHUDWidget = Cast<UPlayerHUD>(CreateWidget(GetWorld(), PlayerHUDClass));
	if(PlayerHUDWidget == nullptr)
		return;
	PlayerHUDWidget->AddToViewport();

	// 캐릭터 BP 찾아놓기
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _WarriorBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerWarrior.BP_PlayerWarrior_C'"));
	if(_WarriorBP.Succeeded())
		WarriorBP = _WarriorBP.Class;
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _MageBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerMage.BP_PlayerMage_C'"));
	if(_MageBP.Succeeded())
		MageBP = _MageBP.Class;
	static ConstructorHelpers::FClassFinder<APlayableCharacter> _GunnerBP(TEXT("Blueprint'/Game/_Blueprints/Player/BP_PlayerGunner.BP_PlayerGunner_C'"));
	if(_GunnerBP.Succeeded())
		GunnerBP = _GunnerBP.Class;
	
}

void APortFolioGameModeBase::BeginPlay()
{
	Super::BeginPlay();


	// 플레이어 캐릭터 생성하기(선택했다면)
	UGameManagerInstance* _GameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	if(_GameInstance->HasSelectedClass == true)
	{
		// 1. 이전에 선택한 클래스
		EPlayerClass _SelectClassType = _GameInstance->GetPlayerClass();

		// 2. 현재 캐릭터 폰 빙의 해제, 없애기(+ 위치 기억)
		APlayerController* _CurrentController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		APawn* _CurrentPlayer = _CurrentController->GetPawn();
		FVector _OriginLocation = _CurrentPlayer->GetActorLocation();
		FRotator _OriginRotation = _CurrentPlayer->GetActorRotation();

		_CurrentController->UnPossess();
		_CurrentPlayer->Destroy();

		// 3. 선택한 캐릭터 액터 생성하기
		TSubclassOf<APlayableCharacter> _SelectedClass = nullptr;
		if(_SelectClassType == EPlayerClass::E_Warrior)
			_SelectedClass = WarriorBP;
		else if(_SelectClassType == EPlayerClass::E_Mage)
			_SelectedClass = MageBP;
		else if(_SelectClassType == EPlayerClass::E_Gunner)
			_SelectedClass = GunnerBP;

		FActorSpawnParameters _SpawnParam = {};
		_SpawnParam.OverrideLevel = GetLevel();
		_SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_SpawnParam.bDeferConstruction = true;
		
		APlayableCharacter* _SpawnPlayer = Cast<APlayableCharacter>(GetWorld()->SpawnActor(_SelectedClass));
		_SpawnPlayer->SetActorLocation(_OriginLocation);
		_SpawnPlayer->SetActorRotation(_OriginRotation);

		// 빙의하기
		_CurrentController->Possess(_SpawnPlayer);
		
	}

	
}
