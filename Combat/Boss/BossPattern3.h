// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossPattern3.generated.h"

UCLASS()
class PORTFOLIO_API ABossPattern3 : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossPattern3();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootScene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* PreParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* ExplodeParticle;

private:
	float AccTime;
	bool bExecuted;

};
