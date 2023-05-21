// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#define GPS UGameplayStatics
#define GPC GPS::GetPlayerController(GetWorld(), 0)
#define GPP GPS::GetPlayerPawn(GetWorld(), 0)
#define GGM GPS::GetGameMode(GetWorld())
#define GPC ZEZUtils::GetPC(GetWorld())
#define GDM ZEZUtils::DM
#define GNotify ZEZUtils::Notify

class ZEZ_API ZEZUtils
{
public:
	ZEZUtils();
	~ZEZUtils();

	/*
		 *  This Util function will write the log for you, so you don't have to type the long code.
		 *  It is the replacement of UE_LOG(LogTemp, Warning, .....)
		 */
	static void PM(FString data, AActor* ActorObject);

	/**
	 * \brief same as above excetp class you can specify
	 * \param data
	 * \param ClassName
	 */
	static void SPM(FString data, FString ClassName);
	/*
	 *  This Util function will Write the Debug Message in game directly so you don't need to check the Log file.
	 *  It is the replacement of PrintString function in BP
	 */
	static void Notify(FString data, AActor* ActorObject);
	/*
	 *  This Util function will Write the Debug Message in game directly so you don't need to check the Log file.
	 *  It is the replacement of PrintString function in BP
	 */
	static void DM(FString data, AActor* ActorObject);
	/*
	 *  This Util function make sphere with the given center, it is for the fast debug purpose.
	 *  Is is the replacement for makeSphere function
	 */
	static void DS(UWorld* WorldObject, FVector Center, AActor* ActorObject);
	
	/*
	 *  This Util function make sphere with the given center, it is for the fast debug purpose.
	 *  Is is the replacement for makeSphere function
	 */
	static void DL(UWorld* WorldObject, FVector StartLoc, FVector EndLoc, AActor* ActorObject);


	/**
	 * \brief This will give you the Class Display Name, so you can get the idea from where Log is printed
	 * \param ActorObject - the object of the class for which you want the Display Name
	 * \return - DisplayName of the parameter
	 */
	static FString GetClass(AActor* ActorObject);

	/**
	 * @brief This will Check if the pointer is valid or not and will print the Message if the pointer is invalid
	 * @param ObjectPointer - the pointer you want to check if it is valid or not
	 * @param Message - Message you want to print, if that is the pointer is not valid
	 * @param ActorObject - the actor in which that pointer exists
	 * @return true if valid pointer else false
	 */
	static bool IsValid(UObject* ObjectPointer, FString Message, AActor* ActorObject);

	/**
	 * @brief This will give you Player Controller 
	 * @param WorldContextObject - world context
	 */
	static APlayerController* GetPC(const UObject* WorldContextObject);
	
	static void PrintAuthority(AActor* ActorObject);
};
