// Copyright © 2023++ Ready Player Me

#pragma once

#include "CoreMinimal.h"
#include "IBaseRequest.h"
#include "AuthorizedRequest.h"
#include "RpmAvatarCreatorTypes.h"

class FRequestFactory
{
public:
	void SetPartnerDomain(const FString& Domain);

	void SetUserData(const FRpmUserData& UserData);

	void SetTokenRefreshedDelegate(const FTokenRefreshed& TokenRefreshed);

	TSharedPtr<IBaseRequest> CreateSendCodeRequest(const FString& PayloadJson) const;
	TSharedPtr<IBaseRequest> CreateConfirmCodeRequest(const FString& PayloadJson) const;
	TSharedPtr<IBaseRequest> CreateTokenRefreshRequest() const;
	TSharedPtr<IBaseRequest> CreateAuthAnonymousRequest() const;
	TSharedPtr<IBaseRequest> CreateAssetRequest() const;
	TSharedPtr<IBaseRequest> CreateColorRequest(const FString& AvatarId) const;
	TSharedPtr<IBaseRequest> CreateAvatarModelRequest(const FString& AvatarId, bool bIsPreview) const;
	TSharedPtr<IBaseRequest> CreateAvatarMetadataRequest(const FString& AvatarId) const;
	TSharedPtr<IBaseRequest> CreateAvatarCreateRequest(const FString& PayloadJson) const;
	TSharedPtr<IBaseRequest> CreateImageRequest(const FString& IconUrl) const;
	TSharedPtr<IBaseRequest> CreateRenderRequest(const FString& AvatarId) const;
	TSharedPtr<IBaseRequest> CreateUserAvatarsRequest() const;
	TSharedPtr<IBaseRequest> CreateUpdateAvatarRequest(const FString& AvatarId, const FString& PayloadJson) const;
	TSharedPtr<IBaseRequest> CreateSaveAvatarRequest(const FString& AvatarId) const;
	TSharedPtr<IBaseRequest> CreateDeleteAvatarRequest(const FString& AvatarId) const;

	TSharedPtr<IBaseRequest> CreateAuthorizedRequest(TSharedPtr<IBaseRequest> MainRequest) const;

private:
	FString PartnerDomain;
	FRpmUserData UserData;
	FTokenRefreshed TokenRefreshedDelegate;
};
