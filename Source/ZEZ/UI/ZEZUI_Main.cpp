// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZUI_Main.h"

#include "ZEZ/Utils/ZEZUtils.h"

void UZEZUI_Main::UpdateCrosshairTexture(bool IsInteractable)
{
	UTexture2D* finalTexture = IsInteractable ? I_CrosshairInteract : I_CrosshairWithoutInteract;
	if(finalTexture)	
		I_Interact->SetBrushFromTexture(finalTexture);
	else
		ZEZUtils::DM("texture not set in UIMain", GetOwningPlayer());
}
