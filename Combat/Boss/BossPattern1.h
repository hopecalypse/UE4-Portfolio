// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossPattern1.generated.h"

class USphereComponent;
UCLASS()
class PORTFOLIO_API ABossPattern1 : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossPattern1();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USphereComponent* RootCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UParticleSystemComponent* Particle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UParticleSystemComponent* HitParticle;

public:
	FVector TargetLocation;

private:
	UFUNCTION()
	void OnOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
