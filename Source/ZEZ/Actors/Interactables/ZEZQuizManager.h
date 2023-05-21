// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "ZEZ/Core/ZEZCharacterBase.h"
#include "ZEZ/Data/S_Question.h"
#include "ZEZQuizManager.generated.h"

UCLASS()
class ZEZ_API AZEZQuizManager : public AActor
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USceneComponent* RootScene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* SM_QuestionWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTextRenderComponent* TR_Question;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* SM_Option1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTextRenderComponent* TR_Option1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* SM_Option2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTextRenderComponent* TR_Option2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* SM_Option3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTextRenderComponent* TR_Option3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* SM_Option4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTextRenderComponent* TR_Option4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USphereComponent* SC_QuizStartArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UInputAction* IA_Shoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UDataTable* DT_Questions;
	UPROPERTY(EditAnywhere) int EasyQuestionCount = 1;
	UPROPERTY(EditAnywhere) int MediumQuestionCount = 1;
	UPROPERTY(EditAnywhere) int HardQuestionCount = 1;

public:

	AZEZQuizManager();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable) void UpdateQuestionInUI(FS_Question Question);
	UFUNCTION(BlueprintCallable) void UpdateQuestion(int DifficultyLevel);
	UFUNCTION(BlueprintCallable) void OnChoiceSelection(int index);

protected:

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent) void CustomEnableInput();	
	UFUNCTION(BlueprintImplementableEvent) void CustomDisableInput();	

private:
	int TotalQuestionAskedCount;
	FS_Question CurrentQuestion;
	AZEZCharacterBase* CurrentInteractingPlayer{nullptr};

private:
	void SetupInputBindings();
	void OnShootPressed();
	void StopQuiz();

	UFUNCTION() void OnQuizAreaOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION() void OnQuizAreaOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
