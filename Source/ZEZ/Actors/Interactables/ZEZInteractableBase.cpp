// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZInteractableBase.h"
#include "iostream"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ZEZ/Core/ZEZCharacterBase.h"
#include "ZEZ/Utils/ZEZUtils.h"


AZEZInteractableBase::AZEZInteractableBase()
{
	PrimaryActorTick.bCanEverTick = true;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootScene->SetupAttachment(RootComponent);
	/*InteractWiget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWiget"));
	InteractWiget->SetupAttachment(RootScene);
	InteractWiget->SetVisibility(false);*/
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootScene);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VisualMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	VisualMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void AZEZInteractableBase::OnHoverTimerOver()
{
	DisableInput(GPC);
	//InteractWiget->SetVisibility(false);	
}

void AZEZInteractableBase::OnHover()
{
	/*if(InteractWiget)
		InteractWiget->SetVisibility(true);*/
	EnableInput(GPC);
}

void AZEZInteractableBase::OnHoverOver()
{
	/*if(InteractWiget)
		InteractWiget->SetVisibility(false);*/
	DisableInput(GPC);
}

void AZEZInteractableBase::PostInteract()
{
	/*if(InteractWiget)
		InteractWiget->SetVisibility(false, true);*/
	if(AZEZCharacterBase* CharacterBase = Cast<AZEZCharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		CharacterBase->ResetInteraction();
	}
}

void AZEZInteractableBase::OnInteractPressed_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "On interaction pressed Interactable base C++");

}

void AZEZInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if(UGameplayStatics::GetPlayerController(GetWorld(),0) && bInteractable)
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = GPC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		EnhancedInputSubsystem->AddMappingContext(IMC_Interact, 1);
	}
}

void AZEZInteractableBase::NFTTimerTick()
{
	NFTMintTimeRemaining -= 1;

	GDM("Time Remaining "+FString::FromInt(NFTMintTimeRemaining), this);

	if(NFTMintTimeRemaining<=0)
	{
		bNFTSet = false;
		NFTMintTimeRemaining = NFTMintTimeDurationConst;
		GDM("Remove the timer text", this);
		GetWorldTimerManager().ClearTimer(NftMintTimerHandle);
		SetOwner(nullptr);
	}
}

void AZEZInteractableBase::S_OnNFTMinTInit_Implementation()
{
	if(!bNFTSet)
	{
		GDM("NFT Mint Initialized, Timer started for "+ FString::SanitizeFloat(NFTMintTimeDurationConst), this);
		bNFTSet = true;
		GetWorld()->GetTimerManager().SetTimer(NftMintTimerHandle, this, &AZEZInteractableBase::NFTTimerTick, 1, true, -1);
	}
}

void AZEZInteractableBase::OnInteraction_Implementation()
{
	GDM("Interaction has to be implemented", this);
}

void AZEZInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZEZInteractableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZEZInteractableBase, bNFTSet);
	DOREPLIFETIME(AZEZInteractableBase, NFTMintTimeRemaining);
}

void AZEZInteractableBase::OnNFTTimeRemainingChanged()
{
	int seconds = NFTMintTimeRemaining;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	FString OutText = FString::FromInt(hours)+" : "+FString::FromInt(minutes)+" : "+FString::FromInt(seconds);
	GDM(OutText, this);
	std::cout << seconds << " seconds is equivalent to " << int(hours) << " hours " << int(minutes % 60)
		<< " minutes " << int(seconds % 60) << " seconds.";
}


