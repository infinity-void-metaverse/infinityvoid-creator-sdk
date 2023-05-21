// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZUI_Interaction.h"

void UZEZUI_Interaction::UpdateInfoText(FString Message)
{
	if(TB_Info && !Message.IsEmpty())
		TB_Info->SetText(FText::FromString(Message));
}
