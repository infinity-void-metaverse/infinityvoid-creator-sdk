// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ReadyPlayerMeComponent.h"
#include "ZEZHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ZEZ/Utils/ZEZUtils.h"

AZEZCharacterBase::AZEZCharacterBase(const FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false; 

	/*ReadyPlayerMeComponent = CreateDefaultSubobject<UReadyPlayerMeComponent>(TEXT("ReadyPlayerMeComponent"));
	AddOwnedComponent(ReadyPlayerMeComponent);*/
}

void AZEZCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AZEZCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if(this->IsLocallyControlled())
	{
		HUDObj = Cast<AZEZHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		GetWorld()->GetTimerManager().SetTimer(CustomTickTimerHandler, this, &AZEZCharacterBase::CustomTick, 0.2, true, 0.2);
	}
}

void AZEZCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZEZCharacterBase, bFlyMode);
}


void AZEZCharacterBase::OnRep_bFlyModeChange()
{
	EMovementMode NewMovementMode = bFlyMode? EMovementMode::MOVE_Flying : MOVE_Walking;
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->SetMovementMode(NewMovementMode);
	if(bFlyMode)
	{
		SetPlayerVisibility(false);
	}
	else
	{
		SetPlayerVisibility(true);
	}
	ZEZUtils::DM("MoveUpdated", this);
}

void AZEZCharacterBase::SetPlayerVisibility(bool Value) const
{
	if(Value)
		GetMesh()->SetVisibility(false, true);
	else
	{
		GetMesh()->SetVisibility(false, false);
	}
}

void AZEZCharacterBase::OnWeaponEquipped(bool NewVisibility)
{
	SetPlayerVisibility(NewVisibility);
}

void AZEZCharacterBase::CustomTick()
{
	//check(HUDObj)
	if(HUDObj && this->IsLocallyControlled())
	{
		FHitResult HitResult;
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector Start = CameraManager->GetCameraLocation();
		FVector End = Start + CameraManager->GetActorForwardVector()* InteractableDistance;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
		if(HitResult.IsValidBlockingHit() && HitResult.GetActor() && GetWorld())
		{
			if( AZEZInteractableBase* InteractableObj = Cast<AZEZInteractableBase>(HitResult.GetActor()))
			{
				if(!InteractableObj->bInteractable)
				{
					PostInteraction();
					return;
				}
				if(CurrentInteractableObj)
					return;
				InteractableObj->OnHover();
				CurrentInteractableObj = InteractableObj;
				HUDObj->UpdateCrosshair(true);
				HUDObj->UpdateInteractionText(CurrentInteractableObj->HUDInteractionText);
			}
			else if(CurrentInteractableObj)
			{
				PostInteraction();
			}
		}
		else
		{
			PostInteraction();
		}
	}
}

void AZEZCharacterBase::PostInteraction()
{
	if(CurrentInteractableObj)
	{
		CurrentInteractableObj->OnHoverOver();
		CurrentInteractableObj = nullptr;
		HUDObj->UpdateCrosshair(false);
	}
}

void AZEZCharacterBase::S_ChangeMovementMode_Implementation(bool bFlyModeValue)
{
	bFlyMode = !bFlyMode;
	OnRep_bFlyModeChange();
}

void AZEZCharacterBase::OnInteractPressed(const FInputActionValue& InputActionValue)
{
	if(CurrentInteractableObj)
	{
		S_UpdateOwnerOfInteractor(CurrentInteractableObj);
		CurrentInteractableObj->OnInteractPressed();
	}
}

void AZEZCharacterBase::S_UpdateOwnerOfInteractor_Implementation(AZEZInteractableBase* InteractableObj)
{
	InteractableObj->SetOwner(this);
}

void AZEZCharacterBase::OnFlyModePressed(const FInputActionValue& InputActionValue)
{
	S_ChangeMovementMode(bFlyMode);
}

void AZEZCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &AZEZCharacterBase::OnInteractPressed);
	EnhancedInputComponent->BindAction(IA_FlyMode, ETriggerEvent::Started, this, &AZEZCharacterBase::OnFlyModePressed);
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZEZCharacterBase::Move);
	EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AZEZCharacterBase::Look);
}

void AZEZCharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AZEZCharacterBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
