// Georgy Treshchev 2023.

#pragma once

#include "Http.h"
#include "Templates/SharedPointer.h"
#include "Launch/Resources/Version.h"
#include "BaseFilesDownloader.generated.h"

/** Dynamic delegate to track download progress */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDownloadProgress, int32, BytesReceived, int32, ContentLength);

/** Static delegate to track download progress */
DECLARE_DELEGATE_TwoParams(FOnDownloadProgressNative, int32, int32);

/** Dynamic delegate to obtain download content length */
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetDownloadContentLength, int32, ContentLength);

/** Static delegate to obtain download content length */
DECLARE_DELEGATE_OneParam(FOnGetDownloadContentLengthNative, int32);

class UTexture2D;

/**
 * Base class for downloading files. It also contains some helper functions
 */
UCLASS(BlueprintType, Category = "Runtime Files Downloader")
class RUNTIMEFILESDOWNLOADER_API UBaseFilesDownloader : public UObject
{
	GENERATED_BODY()

protected:
	/** Static delegate to track download progress */
	FOnDownloadProgressNative OnDownloadProgressNative;

	/** Dynamic delegate to track download progress */
	FOnDownloadProgress OnDownloadProgress;

public:
	/**
	 * File downloading progress internal callback
	 */
	void OnProgress_Internal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) const;

	/**
	 * Canceling the current download
	 *
	 * @return Whether the cancellation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Main")
	bool CancelDownload();

	/**
	 * Get the content length of the file to be downloaded
	 *
	 * @param URL The URL of the file to be downloaded
	 * @param Timeout The maximum time to wait for the download to complete, in seconds. Works only for engine versions >= 4.26
	 * @param OnComplete Delegate for broadcasting the completion of the download 
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Main")
	static void GetContentSize(const FString& URL, float Timeout, const FOnGetDownloadContentLength& OnComplete);

	/**
	 * Get the content length of the file to be downloaded. Suitable for use in C++
	 *
	 * @param URL The URL of the file to be downloaded
	 * @param Timeout The maximum time to wait for the download to complete, in seconds. Works only for engine versions >= 4.26
	 * @param OnComplete Delegate for broadcasting the completion of the download 
	 */
	static void GetContentSize(const FString& URL, float Timeout, const FOnGetDownloadContentLengthNative& OnComplete);

	/**
	 * Convert bytes to string
	 *
	 * @param Bytes Byte array to convert to string
	 * @return Converted string, empty on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static FString BytesToString(const TArray<uint8>& Bytes);

	/**
	 * Convert bytes to texture. This is fully engine-based functionality and may not be well optimized
	 *
	 * @param Bytes Byte array to convert to texture
	 * @return Converted texture or nullptr on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static UTexture2D* BytesToTexture(const TArray<uint8>& Bytes);

	/**
	 * Load a binary file to a dynamic array with two uninitialized bytes at end as padding
	 *
	 * @param FilePath Path to the file to load
	 * @param Result Bytes representation of the loaded file
	 * @return Whether the operation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static bool LoadFileToArray(const FString& FilePath, TArray<uint8>& Result);

	/**
	 * Save a binary array to a file
	 *
	 * @param Bytes Byte array to save to file
	 * @param FilePath Path to the file to save
	 * @return Whether the operation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static bool SaveArrayToFile(const TArray<uint8>& Bytes, const FString& FilePath);

	/**
	 * Load a text file to an FString.
	 * Supports all combination of ANSI/Unicode files and platforms
	 * 
	 * @param Result String representation of the loaded file
	 * @param FilePath Path to the file to load
	 * @return Whether the operation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static bool LoadFileToString(FString& Result, const FString& FilePath);

	/**
	 * Write the string to a file.
	 * Supports all combination of ANSI/Unicode files and platforms
	 *
	 * @param String String to save to file
	 * @param FilePath Path to the file to save
	 * @return Whether the operation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static bool SaveStringToFile(const FString& String, const FString& FilePath);

	/**
	 * Returns true if this file was found, false otherwise
	 *
	 * @param FilePath Path to the file to check
	 * @return Whether the operation was successful or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Files Downloader|Utilities")
	static bool IsFileExist(const FString& FilePath);

protected:
	/** Http download request */
#if ENGINE_MAJOR_VERSION >= 5 || ENGINE_MINOR_VERSION >= 26
	TWeakPtr<IHttpRequest, ESPMode::ThreadSafe> HttpDownloadRequestPtr;
#else
	TWeakPtr<IHttpRequest> HttpDownloadRequestPtr;
#endif

	/** Estimated content length, in bytes. Needed for tracking download progress */
	int32 EstimatedContentLength;

	/**
	 * Broadcast the progress both multi-cast and single-cast delegates
	 * @note To get the download percentage, divide the BytesReceived value by the ContentLength
	 */
	void BroadcastProgress(int32 BytesReceived, int32 ContentLength) const;
};
