// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZSprintBlockVolume.h"

#include "Character/ALSPlayerController.h"
#include "GameFramework/Character.h"

void AZEZSprintBlockVolume::OnJumpBlockColliderBeginOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
                                                             UPrimitiveComponent* PrimitiveComponent1, int I, bool bArg, const FHitResult& HitResult)
{
	if(const ACharacter* GameCharacter = Cast<ACharacter>(Actor))
	{
		if(AALSPlayerController* ALSPlayerController = Cast<AALSPlayerController>(GameCharacter->GetController()))
		{
			ALSPlayerController->CanSprint = false;
		}
	}
}

void AZEZSprintBlockVolume::OnJumpBlockColliderEndOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
	UPrimitiveComponent* PrimitiveComponent1, int I)
{
	if(const ACharacter* GameCharacter = Cast<ACharacter>(Actor))
	{
		if(AALSPlayerController* ALSPlayerController = Cast<AALSPlayerController>(GameCharacter->GetController()))
		{
			ALSPlayerController->CanSprint = true;
		}
	}
}

// Sets default values
AZEZSprintBlockVolume::AZEZSprintBlockVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	JumpBlockBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("JumpBlockBoxCollision"));
	JumpBlockBoxCollision->SetupAttachment(RootComponent);
	JumpBlockBoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	JumpBlockBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	JumpBlockBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AZEZSprintBlockVolume::OnJumpBlockColliderBeginOverlapped);
	JumpBlockBoxCollision->OnComponentEndOverlap.AddDynamic(this, &AZEZSprintBlockVolume::OnJumpBlockColliderEndOverlapped);
	/*VisualBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualBox"));
	VisualBox->SetupAttachment(JumpBlockBoxCollision);
	VisualBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
}

// Called when the game starts or when spawned
void AZEZSprintBlockVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZEZSprintBlockVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

