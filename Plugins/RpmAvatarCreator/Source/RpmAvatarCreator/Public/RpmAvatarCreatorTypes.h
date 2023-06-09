// Copyright © 2023++ Ready Player Me

#pragma once

#include "ReadyPlayerMeTypes.h"

#include "RpmAvatarCreatorTypes.generated.h"

USTRUCT(BlueprintType)
struct FRpmUserData
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	bool bIsAuthenticated;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	bool bIsExistingUser;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Id;

	UPROPERTY()
	FString Token;

	UPROPERTY()
	FString RefreshToken;

	UPROPERTY()
	FString Partner;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Name;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Email;

	FRpmUserData()
	{
		bIsAuthenticated = false;
		bIsExistingUser = false;
	}
};

UENUM(BlueprintType)
enum class ERpmPartnerAssetType : uint8
{
	BeardStyle,
	EyeColor,
	EyeShape,
	EyebrowStyle,
	FaceMask,
	FaceShape,
	Glasses,
	HairStyle,
	Headwear,
	Facewear,
	LipShape,
	NoseShape,
	Outfit,
	Shirt
};

UENUM(BlueprintType)
enum class ERpmPartnerAssetColor : uint8
{
	SkinColor,
	HairColor,
	BeardColor,
	EyebrowColor
};

USTRUCT(BlueprintType)
struct FRpmColorPalette
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	ERpmPartnerAssetColor AssetColor;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	TArray<FColor> RgbColors;

	FRpmColorPalette()
	{
		AssetColor = ERpmPartnerAssetColor::SkinColor;
	}
};

USTRUCT(BlueprintType)
struct FRpmPartnerAsset
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	int64 Id;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Name;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	ERpmPartnerAssetType AssetType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	EAvatarGender Gender;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Icon;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Model;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FString Badge;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	UTexture2D* IconTexture;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	UTexture2D* BadgeTexture;

	FRpmPartnerAsset()
	{
		Id = 0;
		AssetType = ERpmPartnerAssetType::FaceShape;
		Gender = EAvatarGender::Undefined;
		IconTexture = nullptr;
		BadgeTexture = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FRpmAvatarProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Partner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	EAvatarGender Gender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	EAvatarBodyType BodyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TMap<ERpmPartnerAssetColor, int32> Colors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TMap<ERpmPartnerAssetType, int64> Assets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Base64Image;

	FRpmAvatarProperties()
	{
		Gender = EAvatarGender::Undefined;
		BodyType = EAvatarBodyType::Undefined;
	}
};

USTRUCT(BlueprintType)
struct FRpmDefaultAvatarData
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	FRpmAvatarProperties Properties;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	UTexture2D* Image;

	FRpmDefaultAvatarData()
	{
		Image = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FRpmUserAvatar
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Partner;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ready Player Me")
	UTexture2D* Image;

	FRpmUserAvatar()
	{
		Image = nullptr;
	}
};

UENUM(BlueprintType)
enum class ERpmAvatarCreatorError : uint8
{
	None,
	AuthenticationFailure,
	SendActivationCodeFailure,
	ConfirmActivationCodeFailure,
	AssetDownloadFailure,
	ColorDownloadFailure,
	DefaultAvatarFailure,
	UserAvatarDownloadFailure,
	MetadataDownloadFailure,
	AvatarCreateFailure,
	AvatarPreviewFailure,
	AvatarSaveFailure,
	AvatarDeleteFailure
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FAvatarCreatorFailed, ERpmAvatarCreatorError, Error);

DECLARE_DYNAMIC_DELEGATE(FAuthenticationCompleted);

DECLARE_DYNAMIC_DELEGATE_OneParam(FAvatarSaveCompleted, const FString&, Url);

DECLARE_DYNAMIC_DELEGATE(FAvatarDeleteCompleted);

DECLARE_DYNAMIC_DELEGATE(FAvatarEditorReady);

DECLARE_DYNAMIC_DELEGATE_OneParam(FUserAvatarsDownloadCompleted, const TArray<FRpmUserAvatar>&, UserAvatars);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FUserAvatarImageDownloadCompleted, const FString&, AvatarId, UTexture2D*, Image);

DECLARE_DYNAMIC_DELEGATE_OneParam(FDefaultAvatarsDownloadCompleted, const TArray<FRpmDefaultAvatarData>&, DefaultAvatars);

DECLARE_DELEGATE_OneParam(FBaseRequestCompleted, bool);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPreviewDownloadCompleted, const USkeletalMesh*, SkeletalMesh);

DECLARE_LOG_CATEGORY_EXTERN(LogRpmAvatarCreator, Log, All);
