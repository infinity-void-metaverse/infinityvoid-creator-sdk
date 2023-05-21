// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZQuizManager.h"

#include "Kismet/GameplayStatics.h"
#include "ZEZ/Data/S_Question.h"
#include "ZEZ/Utils/ZEZUtils.h"
#include "EnhancedInputComponent.h"
#include "Engine/InputDelegateBinding.h"
#include "Kismet/KismetMathLibrary.h"
#include "ZEZ/Core/ZEZCharacterBase.h"


// Sets default values
AZEZQuizManager::AZEZQuizManager()
{

	PrimaryActorTick.bCanEverTick = false;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootScene->SetupAttachment(RootComponent);

	SC_QuizStartArea = CreateDefaultSubobject<USphereComponent>(TEXT("SC_QuizStartArea"));
	SC_QuizStartArea->SetupAttachment(RootScene);
	SC_QuizStartArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SC_QuizStartArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SC_QuizStartArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SM_QuestionWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	SM_QuestionWall->SetupAttachment(RootScene);
	SM_QuestionWall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TR_Question = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TR_Question"));
	TR_Question->SetupAttachment(SM_QuestionWall);
	
	SM_Option1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Option1"));
	SM_Option1->SetupAttachment(SM_QuestionWall);
	SM_Option1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TR_Option1 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TR_Option1"));
	TR_Option1->SetupAttachment(SM_Option1);
	
	SM_Option2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Option2"));
	SM_Option2->SetupAttachment(SM_QuestionWall);
	SM_Option2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TR_Option2 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TR_Option2"));
	TR_Option2->SetupAttachment(SM_Option2);
	
	SM_Option3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Option3"));
	SM_Option3->SetupAttachment(SM_QuestionWall);
	SM_Option3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TR_Option3 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TR_Option3"));
	TR_Option3->SetupAttachment(SM_Option3);
	
	SM_Option4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Option4"));
	SM_Option4->SetupAttachment(SM_QuestionWall);
	SM_Option4->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TR_Option4 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TR_Option4"));
	TR_Option4->SetupAttachment(SM_Option4);
}

void AZEZQuizManager::UpdateQuestionInUI(FS_Question Question)
{
	TR_Question->SetText(FText::FromString(Question.Question));
	TR_Option1->SetText(FText::FromString(Question.Choice1));
	TR_Option2->SetText(FText::FromString(Question.Choice2));
	TR_Option3->SetText(FText::FromString(Question.Choice3));
	TR_Option4->SetText(FText::FromString(Question.Choice4));
	CurrentQuestion = Question;
}

void AZEZQuizManager::UpdateQuestion(int DifficultyLevel)
{
	if(DT_Questions)
	{
		TotalQuestionAskedCount++;
		//get row no of given difficulty
		TArray<int> QuestionIndexContainerOfDifficulty;
		for(int QuestionIndex = 0; QuestionIndex < 5; QuestionIndex++ )	//TODO update this 5 to 100 as question will be 100
		{
			FS_Question* TempQuestion = DT_Questions->FindRow<FS_Question>(DT_Questions->GetRowNames()[QuestionIndex], "");
			if(TempQuestion->DifficultyLevel == DifficultyLevel)
			{
				QuestionIndexContainerOfDifficulty.Add(QuestionIndex);
			}
		}
		if(QuestionIndexContainerOfDifficulty.Num()==0)
		{
			ZEZUtils::Notify("No questions remaining", this);
			return;
		}
		FS_Question* SelectedQuestion = DT_Questions->FindRow<FS_Question>(DT_Questions->GetRowNames()[
			QuestionIndexContainerOfDifficulty[UKismetMathLibrary::RandomIntegerInRange(0,QuestionIndexContainerOfDifficulty.Num()-1)]], "");
		if (ZEZUtils::IsValid((UObject*)SelectedQuestion,"Row not found in AirDropItems",this ))
		{
			UpdateQuestionInUI(*SelectedQuestion);
		}
	}
}

void AZEZQuizManager::OnChoiceSelection(int index)
{
	if(index== CurrentQuestion.CorrectIndex)
	{
		ZEZUtils::DM("COrrect", this);
	}
	else
	{
		ZEZUtils::DM("In Correct", this);
	}
}

void AZEZQuizManager::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority() && !GPC)
	{
		SC_QuizStartArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		SC_QuizStartArea->OnComponentBeginOverlap.AddDynamic(this, &AZEZQuizManager::OnQuizAreaOverlapBegin);
		SC_QuizStartArea->OnComponentEndOverlap.AddDynamic(this, &AZEZQuizManager::OnQuizAreaOverlapEnd);

		InputComponent = NewObject<UInputComponent>(this);
		InputComponent->RegisterComponent();
		InputComponent->bBlockInput = bBlockInput;
		InputComponent->Priority = InputPriority;
		UInputDelegateBinding::BindInputDelegatesWithSubojects(this, InputComponent);
		SetupInputBindings();
	}
}

void AZEZQuizManager::SetupInputBindings()
{
	if(InputComponent)
		InputComponent->BindAction(FName("Shoot"), EInputEvent::IE_Pressed, this, &AZEZQuizManager::OnShootPressed);
}

void AZEZQuizManager::OnShootPressed()
{
	ZEZUtils::DM("OnShootPressed", this);
	USceneComponent* CameraComponent = GPS::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent();
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = CameraComponent->GetForwardVector()*10000 + Start;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	if(HitResult.IsValidBlockingHit())
	{
		if(Cast<AZEZQuizManager>(HitResult.GetActor()))
		{

			FString HitComponentName = UKismetSystemLibrary::GetDisplayName(HitResult.GetComponent());
			if (HitComponentName.Contains("Option1") && CurrentQuestion.CorrectIndex == 1 ||
				HitComponentName.Contains("Option2") && CurrentQuestion.CorrectIndex == 2 ||
				HitComponentName.Contains("Option3") && CurrentQuestion.CorrectIndex == 3 ||
				HitComponentName.Contains("Option4") && CurrentQuestion.CorrectIndex == 4	)
			{
				GNotify("You hit the Correct Target", this);
				GNotify("Updating Question for you", this);
				if (TotalQuestionAskedCount >= (EasyQuestionCount + MediumQuestionCount + HardQuestionCount))
				{
					GNotify("You have Answered all the questions, Congratulations", this);
				}
				else
				{
					int CurrentDifficultyLevel = 0;
					if((TotalQuestionAskedCount+1) > EasyQuestionCount && (TotalQuestionAskedCount+1) <= (EasyQuestionCount + MediumQuestionCount))
					{
						CurrentDifficultyLevel = 1;
					}
					else if((TotalQuestionAskedCount+1) <= (EasyQuestionCount + MediumQuestionCount + HardQuestionCount) && (TotalQuestionAskedCount+1) > (EasyQuestionCount + MediumQuestionCount))
					{
						CurrentDifficultyLevel = 2;
					}
					UpdateQuestion(CurrentDifficultyLevel);
				}
			}
			else
			{
				GNotify("You hit the wrong answer, Try it again", this);
			}
		}
	}
}

void AZEZQuizManager::OnQuizAreaOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(APawn* PawnInteracted = Cast<APawn>(OtherActor))
	{
		if(PawnInteracted->IsLocallyControlled())
		{
			GNotify("Quiz has started as you have overlapped to quiz area", this);
			EnableInput(GPC);
			if(AZEZCharacterBase* playerCharacter = Cast<AZEZCharacterBase>(OtherActor))
			{
				playerCharacter->bEquippedWeapon = true;
				playerCharacter->OnWeaponEquipped(true);
				CurrentInteractingPlayer = playerCharacter;
			}
			GNotify("Updating Question for you with Easy", this);
			UpdateQuestion(0);
		}
	}
}

void AZEZQuizManager::OnQuizAreaOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(APawn* PawnInteracted = Cast<APawn>(OtherActor))
	{
		if(PawnInteracted->IsLocallyControlled())
		{
			GNotify("You got out from Quiz area", this);
			StopQuiz();
			DisableInput(GPC);
		}
	}
}

void AZEZQuizManager::StopQuiz()
{
	if(ZEZUtils::IsValid(CurrentInteractingPlayer, "Current Interacting player invalid", this))
	{
		CurrentInteractingPlayer->bEquippedWeapon = false;
		CurrentInteractingPlayer->OnWeaponEquipped(false);
		CurrentInteractingPlayer = nullptr;
	}
}

void AZEZQuizManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TIck is off in constructor
}