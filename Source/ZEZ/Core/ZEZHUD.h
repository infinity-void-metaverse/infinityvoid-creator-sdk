// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZEZ/UI/ZEZUI_Interaction.h"
#include "ZEZ/UI/ZEZUI_Main.h"
#include "ZEZHUD.generated.h"

UCLASS()
class ZEZ_API AZEZHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) TSubclassOf<UZEZUI_Main> MainUIClass;
	UPROPERTY(EditAnywhere) TSubclassOf<UZEZUI_Interaction> UIInteractionClass;
	
public:
	AZEZHUD();
	void UpdateCrosshair(bool IsInteractable);
	void UpdateInteractionWidgetVisibility(bool Value);
	void UpdateInteractionText(FString TextValue);
	
	
private:
	UZEZUI_Main* MainUIObj;
	UZEZUI_Interaction* UIInteractionObj;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
