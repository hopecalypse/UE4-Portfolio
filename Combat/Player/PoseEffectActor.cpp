// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Player/PoseEffectActor.h"

#include "Components/PoseableMeshComponent.h"
#include "GameFramework/Character.h"

APoseEffectActor::APoseEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 생성
	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("Poseable Mesh Component"));

	// 머티리얼 에셋 찾기
	static ConstructorHelpers::FObjectFinder<UMaterial> _PoseEffectMat(TEXT("Material'/Game/_Materials/Player/MT_PoseEffect.MT_PoseEffect'"));
	if(_PoseEffectMat.Succeeded())
		PoseMaterial = _PoseEffectMat.Object;

	// 계층 설정
	SetRootComponent(PoseableMesh);
	
}

void APoseEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APoseEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 시간에 따라 Material의 알파값 감소
	AccLifeTime += DeltaTime;
	float _Alpha = 1 - (AccLifeTime / GetLifeSpan());
	PoseableMesh->SetScalarParameterValueOnMaterials(TEXT("Alpha"), _Alpha);
}

void APoseEffectActor::InitPoseEffect(ACharacter* _CopyCharacter, float _LastTime)
{
	// 캐릭터의 Skeletal Mesh 가져오기
	PoseableMesh->SetSkeletalMesh(_CopyCharacter->GetMesh()->SkeletalMesh);

	// 모든 머티리얼에 Effect 할당하기
	UINT _MaterialCount = PoseableMesh->GetNumMaterials();
	for(UINT i = 0; i <_MaterialCount; i++)
		PoseableMesh->CreateDynamicMaterialInstance(i, PoseMaterial);

	// 포즈 복사하기
	PoseableMesh->CopyPoseFromSkeletalComponent(_CopyCharacter->GetMesh());
	
	SetLifeSpan(_LastTime);
}

