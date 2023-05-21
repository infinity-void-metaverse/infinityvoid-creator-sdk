// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZHUD.h"

#include "ZEZ/Utils/ZEZUtils.h"


AZEZHUD::AZEZHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZEZHUD::UpdateCrosshair(bool IsInteractable)
{
	if(MainUIObj)
		MainUIObj->UpdateCrosshairTexture(IsInteractable);
	UpdateInteractionWidgetVisibility(IsInteractable);
}

void AZEZHUD::UpdateInteractionWidgetVisibility(bool Value)
{
	try
	{
		//check(UIInteractionObj)
		if(true)
		{
			if(Value /*&& !UIInteractionObj->IsVisible()*/)
			{
				if(ZEZUtils::IsValid(UIInteractionClass, "UIInteractionClass not defined", this))
				{
					UIInteractionObj = (UZEZUI_Interaction*)CreateWidget<UZEZUI_Interaction>(GetOwningPlayerController(), UIInteractionClass);
					UIInteractionObj->AddToViewport(0);
				}
				
			}
			else if(UIInteractionObj &&  UIInteractionObj->IsVisible())
			{
				UIInteractionObj->RemoveFromParent();
				UIInteractionObj = nullptr;
			}
		}
	}
	catch (...)
	{
		ZEZUtils::DM("UIINteraction Obj has some garbage value", this);
	}
	
}

void AZEZHUD::UpdateInteractionText(FString TextValue)
{
	if(UIInteractionObj && UIInteractionObj->IsVisible())
	{
		UIInteractionObj->UpdateInfoText(TextValue);
	}
}

void AZEZHUD::BeginPlay()
{
	Super::BeginPlay();
	if(ZEZUtils::IsValid(MainUIClass, "Main UI Class not defined", this))
	{
		MainUIObj = CreateWidget<UZEZUI_Main>(GetOwningPlayerController(), MainUIClass);
		MainUIObj->AddToViewport();
	}

}

void AZEZHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

