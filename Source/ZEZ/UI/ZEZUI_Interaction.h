// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "ZEZUI_Interaction.generated.h"

/**
 * 
 */
UCLASS()
class ZEZ_API UZEZUI_Interaction : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UCanvasPanel* CP_Main;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UCanvasPanel* CP_Interact;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UImage* I_Interact;
	UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadOnly) UTextBlock* TB_Info;

public:
	void UpdateInfoText(FString Message);
};

