// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USphereComponent;
UCLASS()
class PORTFOLIO_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	// 필요 컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USphereComponent* RootSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

};
