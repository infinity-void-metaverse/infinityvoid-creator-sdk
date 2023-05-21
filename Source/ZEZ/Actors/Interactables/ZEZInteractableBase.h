// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "ZEZ/Actors/Interfaces/ZEZIInteract.h"
#include "ZEZInteractableBase.generated.h"

UCLASS()
class ZEZ_API AZEZInteractableBase : public AActor, public IZEZIInteract
{
public:

private:
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) USceneComponent* RootScene;
	// UPROPERTY(EditAnywhere) UBoxComponent* InteractableBox;
	UPROPERTY(EditAnywhere) UInputMappingContext* IMC_Interact;
	UPROPERTY(EditAnywhere) UInputAction* IA_Interact;
	//UPROPERTY(EditAnywhere) TSubclassOf<UUserWidget> UWInteractionClass;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly) UWidgetComponent* InteractWiget;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* VisualMesh;
	UPROPERTY(EditAnywhere) int HoverTimerWait = 5;
	UPROPERTY(EditAnywhere) FString HUDInteractionText = "Interact";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bInteractable {true};
	UPROPERTY(EditAnywhere) int NFTMintTimeDurationConst {(int)(.5*60*60)};	//In Seconds - hh*mm*ss
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnNFTTimeRemainingChanged) int NFTMintTimeRemaining {NFTMintTimeDurationConst};	//In Seconds - hh*mm*ss
	UPROPERTY(BlueprintReadWrite, Replicated) bool bNFTSet {false};

public:
	AZEZInteractableBase();
	void OnHover();
	void OnHoverOver();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent)	void OnInteractPressed();
	UFUNCTION(BlueprintCallable) void PostInteract();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	UFUNCTION() void OnNFTTimeRemainingChanged();

protected:
	void OnHoverTimerOver();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent) void OnInteraction();
	UFUNCTION(Server, Reliable, BlueprintCallable) void S_OnNFTMinTInit();
	
private:
	FTimerHandle NftMintTimerHandle;
	//UUserWidget* UWInteraction;
	
private:
	UFUNCTION() void NFTTimerTick();
	
};

