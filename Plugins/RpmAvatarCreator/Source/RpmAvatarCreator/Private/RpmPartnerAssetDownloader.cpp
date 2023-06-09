// Copyright © 2023++ Ready Player Me


#include "RpmPartnerAssetDownloader.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Requests/RequestFactory.h"
#include "Extractors/PartnerAssetExtractor.h"

namespace
{
	bool IsAssetFiltered(const FRpmPartnerAsset& Asset, EAvatarBodyType BodyType, EAvatarGender Gender)
	{
		const bool BodyTypeFiltered = !((Asset.AssetType == ERpmPartnerAssetType::Outfit && BodyType == EAvatarBodyType::HalfBody) ||
			(Asset.AssetType == ERpmPartnerAssetType::Shirt && BodyType == EAvatarBodyType::FullBody));
		const bool GenderFiltered = Asset.Gender == Gender || Asset.Gender == EAvatarGender::Undefined;
		return BodyTypeFiltered && GenderFiltered;
	}
}

void URpmPartnerAssetDownloader::SetRequestFactory(TSharedPtr<class FRequestFactory> Factory)
{
	RequestFactory = Factory;
}

void URpmPartnerAssetDownloader::DownloadAssets()
{
	AssetRequest = RequestFactory->CreateAssetRequest();
	AssetRequest->GetCompleteCallback().BindUObject(this, &URpmPartnerAssetDownloader::OnAssetsDownloadCompleted);
	AssetRequest->Download();
}

TArray<FRpmPartnerAsset> URpmPartnerAssetDownloader::GetFilteredAssets(EAvatarBodyType BodyType, EAvatarGender Gender) const
{
	return Assets.FilterByPredicate([BodyType, Gender](const auto& Asset){ return IsAssetFiltered(Asset, BodyType, Gender); });
}

bool URpmPartnerAssetDownloader::AreAssetsReady() const
{
	return IconRequests.Num() == 0 && !AssetRequest.IsValid();
}

FBaseRequestCompleted& URpmPartnerAssetDownloader::GetPartnerAssetsDownloadCallback()
{
	return OnPartnerAssetsDownloaded;
}

FBaseRequestCompleted& URpmPartnerAssetDownloader::GetIconsDownloadCallback()
{
	return OnIconsDownloaded;
}

void URpmPartnerAssetDownloader::OnAssetsDownloadCompleted(bool bSuccess)
{
	if (bSuccess)
	{
		Assets = FPartnerAssetExtractor::ExtractAssets(AssetRequest->GetContentAsString());
	}
	AssetRequest.Reset();
	(void)OnPartnerAssetsDownloaded.ExecuteIfBound(Assets.Num() != 0);
	OnPartnerAssetsDownloaded.Unbind();
}

void URpmPartnerAssetDownloader::DownloadIcons(EAvatarBodyType BodyType, EAvatarGender Gender)
{
	for (const auto& Asset : Assets)
	{
		if (IsValid(Asset.IconTexture) || !IsAssetFiltered(Asset, BodyType, Gender))
		{
			continue;
		}
		auto IconRequest = RequestFactory->CreateImageRequest(Asset.Icon);
		IconRequests.Add(Asset.Icon, IconRequest);
		IconRequest->GetCompleteCallback().BindUObject(this, &URpmPartnerAssetDownloader::OnIconDownloadCompleted, Asset.Icon);
		IconRequest->Download();
		if (!Asset.Badge.IsEmpty())
		{
			auto BadgeRequest = RequestFactory->CreateImageRequest(Asset.Badge);
			if (!IconRequests.Contains(Asset.Badge))
			{
				IconRequests.Add(Asset.Badge, BadgeRequest);
			}
			BadgeRequest->GetCompleteCallback().BindUObject(this, &URpmPartnerAssetDownloader::OnIconDownloadCompleted, Asset.Badge);
			BadgeRequest->Download();
		}
	}
	if (IconRequests.Num() == 0)
	{
		(void)OnIconsDownloaded.ExecuteIfBound(true);
		OnIconsDownloaded.Unbind();
	}
}

void URpmPartnerAssetDownloader::OnIconDownloadCompleted(bool bSuccess, FString Icon)
{
	if (!bSuccess)
	{
		(void)OnIconsDownloaded.ExecuteIfBound(false);
		OnIconsDownloaded.Unbind();
		// Check if we need to stop downloading other assets
		return;
	}

	UTexture2D* Texture = UKismetRenderingLibrary::ImportBufferAsTexture2D(this, IconRequests[Icon]->GetContent());
	IconRequests.Remove(Icon);
	ImageMap.Add(Icon, Texture);

	if (IconRequests.Num() == 0)
	{
		for (auto& Asset : Assets)
		{
			if (ImageMap.Contains(Asset.Icon))
			{
				Asset.IconTexture = ImageMap[Asset.Icon];
			}
			if (ImageMap.Contains(Asset.Badge))
			{
				Asset.BadgeTexture = ImageMap[Asset.Badge];
			}
		}
		ImageMap.Empty();
		(void)OnIconsDownloaded.ExecuteIfBound(true);
		OnIconsDownloaded.Unbind();
	}
}
