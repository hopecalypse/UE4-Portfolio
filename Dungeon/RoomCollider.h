// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomCollider.generated.h"

class UDungeonRoom;
class UBoxComponent;
UCLASS()
class PORTFOLIO_API ARoomCollider : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomCollider();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;


public:
	UBoxComponent* ColliderBox;
	UDungeonRoom* Room;

	UFUNCTION()
	void OnOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
