// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "S_Question.generated.h"

// This class does not need to be modified.
USTRUCT(BlueprintType)
struct FS_Question : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Question;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Choice1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Choice2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Choice3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Choice4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CorrectIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int DifficultyLevel;	//0 - Easy, 1 - Med, 2 - Hard
};
