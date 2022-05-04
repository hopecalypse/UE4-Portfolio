// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AudioDataAsset.generated.h"

class USoundCue;
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UAudioDataAsset : public UDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundAttenuation* Attenuation;
	// Warrior
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Warrior_SwordQueue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Warrior_Skill1Impact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Warrior_Skill2Voice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* Warrior_Skill2;

	// Gunner
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Gunner_GrenadeExplode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Gunner_RocketLaunch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Gunner_RocketHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Gunner_LaserLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Gunner_LaserHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* LevelUp;

	// Monster
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* Monster_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* Monster_Shoot;

	// Boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill1Hit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill2Ready;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill2Impact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill3Cast;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill3Clap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill3Ball;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Boss_Skill3Hit;
};
