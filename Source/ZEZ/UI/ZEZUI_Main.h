// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "ZEZUI_Main.generated.h"

/**
 * 
 */
UCLASS()
class ZEZ_API UZEZUI_Main : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UCanvasPanel* CP_Main;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UCanvasPanel* CP_Crosshair;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UImage* I_Interact;
	UPROPERTY(EditAnywhere) UTexture2D* I_CrosshairInteract;
	UPROPERTY(EditAnywhere) UTexture2D* I_CrosshairWithoutInteract;
	//UPROPERTY(EditAnywhere, meta = (BindWidget)) UBorder* B_Center;

public:
	UFUNCTION(BlueprintCallable) void UpdateCrosshairTexture(bool IsInteractable);
};
