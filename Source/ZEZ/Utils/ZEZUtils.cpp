// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEZUtils.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ZEZUtils::PM(FString data, AActor* ActorObject)
{
	//GEngine->AddOnScreenDebugMessage(1,1, FColor::Red, data);
	data = "Classs: " + GetClass(ActorObject) + " " + data;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *data);

}

void ZEZUtils::SPM(FString data, FString ClassName)
{
	//GEngine->AddOnScreenDebugMessage(1,1, FColor::Red, data);
	data = "Classs: " + ClassName + " " + data;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *data);

}

void ZEZUtils::Notify(FString data, AActor* ActorObject)
{
	DM(data, ActorObject);
}

void ZEZUtils::DM(FString data, AActor* ActorObject)
{
	data = "Classs: " + GetClass(ActorObject) + " " + data;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, data);
}

void ZEZUtils::DS(UWorld* WorldObject, FVector Center, AActor* ActorObject)
{
	DrawDebugSphere(WorldObject, Center, 50, 32, FColor::Red, true);
	PM("Drawing Debug Sphere", ActorObject);
}

void ZEZUtils::DL(UWorld* WorldObject, FVector StartLoc, FVector EndLoc, AActor* ActorObject)
{
	DrawDebugLine(WorldObject, StartLoc, EndLoc, FColor::Red, true);	
	PM("Drawing Debug line", ActorObject);
}

FString ZEZUtils::GetClass(AActor* ActorObject)
{
	return UKismetSystemLibrary::GetDisplayName(ActorObject);
}

bool ZEZUtils::IsValid(UObject* ObjectPointer, FString Message, AActor* ActorObject)
{
	if(ObjectPointer)
	{
		return true;
	}
	else
	{
		DM(Message, ActorObject);
		return false;
	}
}

APlayerController* ZEZUtils::GetPC(const UObject* WorldContextObject)
{
	return UGameplayStatics::GetPlayerController(WorldContextObject,0);
}

void ZEZUtils::PrintAuthority(AActor* ActorObject)
{
	if(ActorObject->HasAuthority())
		DM("Has Authority ", ActorObject);
	else
		DM("No Authority ", ActorObject);
}


