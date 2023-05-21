// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ZEZJumpBlockVolume.generated.h"

UCLASS()
class ZEZ_API AZEZJumpBlockVolume : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) UBoxComponent* JumpBlockBoxCollision;
	//UPROPERTY(EditAnywhere) UStaticMeshComponent* VisualBox;
	
public:
	UFUNCTION()
	void OnJumpBlockColliderBeginOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor, UPrimitiveComponent* PrimitiveComponent1, int I, bool bArg,
	                                        const FHitResult& HitResult);
	UFUNCTION()
	void OnJumpBlockColliderEndOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor, UPrimitiveComponent* PrimitiveComponent1, int I);
	// Sets default values for this actor's properties
	AZEZJumpBlockVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
