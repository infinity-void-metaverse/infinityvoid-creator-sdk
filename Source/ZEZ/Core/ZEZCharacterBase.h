// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Character/ALSCharacter.h"
#include "../Actors/Interactables/ZEZInteractableBase.h"
#include "ZEZCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
UCLASS()
class ZEZ_API AZEZCharacterBase : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Custom|Input") UInputAction* IA_Interact;
	UPROPERTY(EditAnywhere, Category="Custom|Input") UInputAction* IA_FlyMode;
	UPROPERTY(EditAnywhere, Category="Custom|Input") UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category="Custom|Input") UInputAction* IA_Jump;
	UPROPERTY(EditAnywhere, Category="Custom|Input") UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category="Custom|Input") class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	class UReadyPlayerMeComponent* ReadyPlayerMeComponent;

	UPROPERTY(EditAnywhere) float InteractableDistance = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEquippedWeapon = false;

public:
	void OnInteractPressed(const FInputActionValue& InputActionValue);
	void OnFlyModePressed(const FInputActionValue& InputActionValue);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void ResetInteraction() { CurrentInteractableObj = nullptr; }
	UFUNCTION(BlueprintCallable) void SetPlayerVisibility(bool Value) const;
	UFUNCTION(BlueprintCallable) void OnWeaponEquipped(bool NewVisibility); 

protected:
	UPROPERTY(ReplicatedUsing = OnRep_bFlyModeChange, BlueprintReadWrite) bool bFlyMode = false;

protected:
	AZEZCharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const override;


private:
	AZEZInteractableBase* CurrentInteractableObj;
	class AZEZHUD* HUDObj;
	FTimerHandle CustomTickTimerHandler;
	
private:
	UFUNCTION(Server, Reliable) void S_ChangeMovementMode(bool bFlyModeValue);
	UFUNCTION(Server, Reliable) void S_UpdateOwnerOfInteractor(AZEZInteractableBase* InteractableObj);
	UFUNCTION()	void OnRep_bFlyModeChange();
	void CustomTick();
	void PostInteraction();
};
