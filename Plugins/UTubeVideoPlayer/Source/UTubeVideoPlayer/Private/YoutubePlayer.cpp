// Copyright 2020 Kelvin Rosa, All Rights Reserved.


#include "YoutubePlayer.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// Sets default values for this component's properties
UYoutubePlayer::UYoutubePlayer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	// ...
}

void UYoutubePlayer::LoadYoutubeUrl(FString _youtubeUrl)
{
	useQuality = false;
	videoEncryptedUrl = "";
	YoutubeServerCall(_youtubeUrl);
}

void UYoutubePlayer::YoutubeServerCall(FString videoId)
{
	TMap<FString, FString> query = ParseQueryString(videoId);
	if(query.Contains("v"))
	{
		videoId = *query.Find("v");
	}
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UYoutubePlayer::OnResponseReceivedNew);
	//This is the url on which to process the request
	FString jsonData = TEXT("{\"context\": {\"client\": {\"clientName\": \"ANDROID\",\"clientVersion\": \"16.20\",\"hl\": \"en\"}},\"videoId\": \""+videoId+"\",}");
	//create byte array of the string
	Request->SetContentAsString(jsonData);
	Request->SetVerb("POST");
	Request->SetURL("https://www.youtube.com/youtubei/v1/player?key=AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8");
	Request->SetHeader(TEXT("X-YouTube-Client-Name"), TEXT("3"));
	Request->SetHeader(TEXT("X-YouTube-Client-Version"), TEXT("16.20"));
	//Request->SetHeader("User-Agent", TEXT("Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0 (Chrome)"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	//Request->SetContent(ResultChars);
	Request->ProcessRequest();
}

void UYoutubePlayer::LoadYoutubeUrlMultiquality(FString _youtubeUrl, TEnumAsByte<VideoQuality> _quality)
{
	switch (_quality)
	{
	case qual_sd:
        selectedItag = 18;
		videoEncryptedUrl = "";
		useQuality = false;
		break;
	case qual_hd:
        selectedItag = 136;
		useQuality = true;
		break;
	case qual_fullhd:
        selectedItag = 137;
		useQuality = true;
		break;
	case qual_none:
        selectedItag = 18;
		videoEncryptedUrl = "";
		useQuality = false;
		break;
	default:
        selectedItag = 18;
		useQuality = true;
		break;
	}

	MyHttpCall(_youtubeUrl);
}

/*Http call*/
void UYoutubePlayer::MyHttpCall(FString _youtubeUrl)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UYoutubePlayer::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(_youtubeUrl);
	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", TEXT("Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0 (Chrome)"));
	Request->SetHeader("Content-Type", TEXT("text/html"));
	Request->ProcessRequest();
}

bool UYoutubePlayer::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	else {
		UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
		return false;
	}
}

bool UYoutubePlayer::IsVideoUnavailable(FString pageSource)
{
	FString unavailableContainer = "<div id=\"watch-player-unavailable\">";
	return pageSource.Contains(unavailableContainer);
}

void UYoutubePlayer::OnResponseReceivedNew(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (ResponseIsValid(Response, bWasSuccessful))
	{
		FString pageContent = Response->GetContentAsString();
		if (IsVideoUnavailable(pageContent))
		{
			UE_LOG(LogTemp, Warning, TEXT("Video Unavailable!"));
			//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, "Video Unavailable!");
		}
		else
		{
			TSharedPtr<FJsonObject> pResponse;
			TSharedRef<TJsonReader<>> Reader2 = TJsonReaderFactory<>::Create(pageContent);

					if (FJsonSerializer::Deserialize(Reader2, pResponse))
					{
						TSharedPtr<FJsonObject> videoDetails = pResponse->GetObjectField("videoDetails");
						FString videoTitle = videoDetails->GetStringField("title");
						bool isLive = videoDetails->GetBoolField("isLive");

						//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, videoTitle);
						UE_LOG(LogTemp, Verbose, TEXT("Title: %s"), *videoTitle);

						TArray<TSharedPtr<FJsonValue>> streamMap = GetStreamMap(pResponse);
						//TArray<FString> splitByUrls;
						//streamMap.ParseIntoArray(splitByUrls, TEXT(","), false);


						TArray<TSharedPtr<FJsonValue>> adaptiveStreamMap = GetAdaptiveStreamMap(pResponse);
						//TArray<FString> adaptiveFmtSplitByUrls;
						//adaptiveStreamMap.ParseIntoArray(adaptiveFmtSplitByUrls, TEXT(","), false);

						streamMap.Append(adaptiveStreamMap);

						TArray<FString> splitByUrls;

						if(!isLive)
						{
							for (int32 index = 0; index < streamMap.Num(); index++)
							{
								TSharedPtr<FJsonObject> obj = streamMap[index]->AsObject();
								/*FString OutputString;
								TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
								FJsonSerializer::Serialize(obj.ToSharedRef(), Writer);

								UE_LOG(LogTemp, Warning, TEXT("resulting jsonString -> %s"), *OutputString);
								*/
								int32 itag = obj->GetIntegerField("itag");
								if (itag == 18) //video quality.
								{
									bool requiresDecryption = false;
									FString url;

									FString signature;
									FString signatureQuery = "sig";
									if (obj->TryGetStringField("cipher", signature))
									{
										requiresDecryption = true;

										FString cipher = obj->GetStringField("cipher");
										TMap<FString, FString> queries = ParseQueryString(cipher);

										if (queries.Contains("s") || queries.Contains("signature"))
										{
											requiresDecryption = queries.Contains("s");
											if (queries.Contains("s"))
											{
												signature = *queries.Find("s");
											}
											else
											{
												signature = *queries.Find("signature");
											}


											if (queries.Contains("sp"))
												signatureQuery = "sig";
											else
												signatureQuery = "signatures";

											url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

											if (queries.Contains("fallback_host"))
											{
												url += "&fallback_host=" + *queries.Find("fallback_host");
											}

										}
									}else if(obj->TryGetStringField("signatureCipher", signature))
									{
										requiresDecryption = true;

										FString cipher = obj->GetStringField("signatureCipher");
										TMap<FString, FString> queries = ParseQueryString(cipher);

										if (queries.Contains("s") || queries.Contains("signature"))
										{
											requiresDecryption = queries.Contains("s");
											if (queries.Contains("s"))
											{
												signature = *queries.Find("s");
											}
											else
											{
												signature = *queries.Find("signature");
											}


											if (queries.Contains("sp"))
												signatureQuery = "sig";
											else
												signatureQuery = "signatures";

											url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

											if (queries.Contains("fallback_host"))
											{
												url += "&fallback_host=" + *queries.Find("fallback_host");
											}

										}
									}
									else 
									{
										url = obj->GetStringField("url");
									}

									url = FGenericPlatformHttp::UrlDecode(url);

									TMap<FString, FString> parameters = ParseQueryString(url);
									if (!parameters.Contains("ratebypass"))
									{
										url += "&ratebypass=yes";
									}

									if(useQuality == false)
									{
										if (!requiresDecryption) OnUrlLoaded.Broadcast(*url);
										else
										{
											encryptedUrl = url;
											//DecryptVideoUrl(encryptedUrl,"", htmlVersion);
										}
									}
									else {
										encryptedUrl = url;
									}

									UE_LOG(LogTemp, Warning, TEXT("signature:  -> %s"), *signature);
								}
							}

							if (useQuality) 
							{
								for (int32 index = 0; index < streamMap.Num(); index++)
								{
									TSharedPtr<FJsonObject> obj = streamMap[index]->AsObject();
									
									int32 itag = obj->GetIntegerField("itag");
									if (itag == selectedItag) //video quality.
									{
										
										bool requiresDecryption = false;
										FString url;

										FString signature;
										FString signatureQuery = "sig";
										if (obj->TryGetStringField("cipher", signature))
										{
											requiresDecryption = true;

											FString cipher = obj->GetStringField("cipher");
											TMap<FString, FString> queries = ParseQueryString(cipher);

											if (queries.Contains("s") || queries.Contains("signature"))
											{
												requiresDecryption = queries.Contains("s");
												if (queries.Contains("s"))
												{
													signature = *queries.Find("s");
												}
												else
												{
													signature = *queries.Find("signature");
												}


												if (queries.Contains("sp"))
													signatureQuery = "sig";
												else
													signatureQuery = "signatures";

												url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

												if (queries.Contains("fallback_host"))
												{
													url += "&fallback_host=" + *queries.Find("fallback_host");
												}

											}
										}else if(obj->TryGetStringField("signatureCipher", signature))
										{
											requiresDecryption = true;

											FString cipher = obj->GetStringField("signatureCipher");
											TMap<FString, FString> queries = ParseQueryString(cipher);

											if (queries.Contains("s") || queries.Contains("signature"))
											{
												requiresDecryption = queries.Contains("s");
												if (queries.Contains("s"))
												{
													signature = *queries.Find("s");
												}
												else
												{
													signature = *queries.Find("signature");
												}


												if (queries.Contains("sp"))
													signatureQuery = "sig";
												else
													signatureQuery = "signatures";

												url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

												if (queries.Contains("fallback_host"))
												{
													url += "&fallback_host=" + *queries.Find("fallback_host");
												}

											}
										}
										else
										{
											url = obj->GetStringField("url");
										}

										url = FGenericPlatformHttp::UrlDecode(url);

										TMap<FString, FString> parameters = ParseQueryString(url);
										if (!parameters.Contains("ratebypass"))
										{
											url += "&ratebypass=yes";
										}
										UE_LOG(LogTemp, Verbose, TEXT("Get second url"));
										if (useQuality)
										{
											if (!requiresDecryption) OnUrlLoadedMultiQuality.Broadcast(*encryptedUrl, *url);
											else
											{
												UE_LOG(LogTemp, Verbose, TEXT("Get second durl"));
												videoEncryptedUrl = url;
												//DecryptVideoUrl(encryptedUrl, videoEncryptedUrl, htmlVersion);
											}
										}
									}
								}
							}
						}else
						{
							TSharedPtr<FJsonObject> streamingData = pResponse->GetObjectField("streamingData");
							FString liveUrl = streamingData->GetStringField("hlsManifestUrl");
							OnUrlLoaded.Broadcast(*liveUrl);
						}
						

						//for (FString s : splitByUrls)
						//{
						//	//UE_LOG(LogTemp, Warning, TEXT("String to parse: %s"), *s);
						//	TMap<FString, FString> queries = ParseQueryString(s);

						//	bool requiresDecryption = false;
						//	FString url;

						//	if (queries.Contains("s") || queries.Contains("signature"))
						//	{
						//		requiresDecryption = queries.Contains("s");
						//		FString signature;
						//		if (queries.Contains("s"))
						//		{
						//			signature = *queries.Find("s");
						//		}
						//		else
						//		{
						//			signature = *queries.Find("signature");
						//		}

						//		FString signatureQuery = "sig";

						//		url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;
						//		if (queries.Contains("fallback_host"))
						//		{
						//			url += "&fallback_host=" + *queries.Find("fallback_host");
						//		}

						//	}
						//	else
						//	{
						//		FString resultURL = *queries.Find("url");
						//		url = *queries.Find("url");
						//	}

						//	url = FGenericPlatformHttp::UrlDecode(url);

						//	UE_LOG(LogTemp, Warning, TEXT("URL: %s"), *url);

						//	TMap<FString, FString> parameters = ParseQueryString(url);

						//	if (!parameters.Contains("ratebypass"))
						//	{
						//		url += "&ratebypass=yes";
						//		//TODO USE THE 18 itag
						//	}

						//	FString itag = *ParseQueryString(url).Find("itag");
						//	if (itag == "18") {
						//		//UE_LOG(LogTemp, Warning, TEXT("ITAG: %s"), *itag);
						//		//UE_LOG(LogTemp, Warning, TEXT("Let's Play: %s"), *url);

						//		if (!requiresDecryption) OnUrlLoaded.Broadcast(*url);
						//		else
						//		{
						//			encryptedUrl = url;
						//			DecryptVideoUrl(encryptedUrl, htmlVersion);
						//		}
						//	}
						//}
					}

				//}
			
		}

		////Create a pointer to hold the json serialized data
		//TSharedPtr<FJsonObject> JsonObject;

		////Create a reader pointer to read the json data
		//TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		////Deserialize the json data given Reader and the actual object to deserialize
		//if (FJsonSerializer::Deserialize(Reader, JsonObject))
		//{
		//	//Get the value of the json object by field name
		//	FString recievedInt = JsonObject->GetStringField("customInt");

		//	//Output it to the engine
		//	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, recievedInt);
		//	OnUrlLoaded.Broadcast("VIDEO QUALQUER");
		//}
	}
	else
	{
		OnGetUrlError.Broadcast("Invalid Response");
	}
}

void UYoutubePlayer::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (ResponseIsValid(Response, bWasSuccessful))
	{
		FString pageContent = Response->GetContentAsString();
		if (IsVideoUnavailable(pageContent))
		{
			UE_LOG(LogTemp, Warning, TEXT("Video Unavailable!"));
			//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, "Video Unavailable!");
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, "Video Available!");
			FString pattern = "ytplayer\\.config\\s*=\\s*(\\{.+?\\});";
			const FRegexPattern myPattern(pattern);
			FRegexMatcher myMatcher(myPattern, pageContent);

			FString p_response = "";
			bool foundPResponse = false;


			if (myMatcher.FindNext())
			{
				FString extractedJson = myMatcher.GetCaptureGroup(1);

				if (!extractedJson.Contains("raw_player_response:ytInitialPlayerResponse"))
				{
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(extractedJson);

					if (FJsonSerializer::Deserialize(Reader, JsonObject))
					{
						TSharedPtr<FJsonObject> args = JsonObject->GetObjectField("args");
						p_response = args->GetStringField("player_response");
						foundPResponse = true;
					}

				}
			}

			//"ytInitialPlayerResponse\s*=\s*({.+?})\s*;\s*(?:var\s+meta|</script|\n)"


			FString patternSecondary = "ytInitialPlayerResponse\\s*=\\s*(\\{.+?\\})\\s*;\\s*(?:var\\s+meta|</script>|\\n)";
			const FRegexPattern myPatternSecondary(patternSecondary);
			FRegexMatcher myMatcherSecondary(myPatternSecondary, pageContent);

			if (myMatcherSecondary.FindNext())
			{
				FString extractedJson = myMatcherSecondary.GetCaptureGroup(1);
				p_response = extractedJson;
				foundPResponse = true;
			}

			//"ytInitialPlayerResponse\s*=\s*({.+?})\s*;"

			FString patternThird = "ytInitialPlayerResponse\\s*=\\s*(\\{.+?\\})\\s*;";
			const FRegexPattern myPatternThird(patternThird);
			FRegexMatcher myMatcherThird(myPatternThird, pageContent);

			if (myMatcherThird.FindNext())
			{
				FString extractedJson = myMatcherThird.GetCaptureGroup(1);
				p_response = extractedJson;
				foundPResponse = true;
			}


			if (foundPResponse)
			{
				/*FString result = myMatcher.GetCaptureGroup(1);

				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(result);*/

				//if (FJsonSerializer::Deserialize(Reader, JsonObject))
				//{
					//TSharedPtr<FJsonObject> args = JsonObject->GetObjectField("args");
					FString player_response = p_response;
					FString htmlVersion = "";
					
					/*if (!JsonObject->HasField("assets"))
					{*/
						FString patternTempForFix = "\"jsUrl\"\\s*:\\s*\"([^\"]+)\"";
						const FRegexPattern myPatternForFix(patternTempForFix);
						FRegexMatcher myMatcherForFix(myPatternForFix, pageContent);
						if (myMatcherForFix.FindNext())
						{
							FString resultForFix = myMatcherForFix.GetCaptureGroup(1);
							
								//FString js = JsonObjectForFix->GetStringField("jsUrl");
							htmlVersion = GetHtml5PlayerVersionForFix(resultForFix);
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("NOT FOUND JS URL Mail kelvinparkour@gmail.com and ask for support."));
						}
					/*}
					else 
					{
						UE_LOG(LogTemp, Warning, TEXT("Normal"));
						htmlVersion = GetHtml5PlayerVersion(JsonObject);
					}*/
					

					
					TSharedPtr<FJsonObject> pResponse;
					TSharedRef<TJsonReader<>> Reader2 = TJsonReaderFactory<>::Create(player_response);

					if (FJsonSerializer::Deserialize(Reader2, pResponse))
					{
						TSharedPtr<FJsonObject> videoDetails = pResponse->GetObjectField("videoDetails");
						FString videoTitle = videoDetails->GetStringField("title");

						//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, videoTitle);
						UE_LOG(LogTemp, Verbose, TEXT("Title: %s"), *videoTitle);

						TArray<TSharedPtr<FJsonValue>> streamMap = GetStreamMap(pResponse);
						//TArray<FString> splitByUrls;
						//streamMap.ParseIntoArray(splitByUrls, TEXT(","), false);


						TArray<TSharedPtr<FJsonValue>> adaptiveStreamMap = GetAdaptiveStreamMap(pResponse);
						//TArray<FString> adaptiveFmtSplitByUrls;
						//adaptiveStreamMap.ParseIntoArray(adaptiveFmtSplitByUrls, TEXT(","), false);

						streamMap.Append(adaptiveStreamMap);

						TArray<FString> splitByUrls;

						for (int32 index = 0; index < streamMap.Num(); index++)
						{
							
							TSharedPtr<FJsonObject> obj = streamMap[index]->AsObject();

							/*FString OutputString;
							TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
							FJsonSerializer::Serialize(obj.ToSharedRef(), Writer);

							UE_LOG(LogTemp, Warning, TEXT("resulting jsonString -> %s"), *OutputString);
							*/
							int32 itag = obj->GetIntegerField("itag");
							if (itag == 18) //video quality.
							{
								bool requiresDecryption = false;
								FString url;

								FString signature;
								FString signatureQuery = "sig";
								if (obj->TryGetStringField("cipher", signature))
								{
									requiresDecryption = true;

									FString cipher = obj->GetStringField("cipher");
									TMap<FString, FString> queries = ParseQueryString(cipher);

									if (queries.Contains("s") || queries.Contains("signature"))
									{
										requiresDecryption = queries.Contains("s");
										if (queries.Contains("s"))
										{
											signature = *queries.Find("s");
										}
										else
										{
											signature = *queries.Find("signature");
										}


										if (queries.Contains("sp"))
											signatureQuery = "sig";
										else
											signatureQuery = "signatures";

										url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

										if (queries.Contains("fallback_host"))
										{
											url += "&fallback_host=" + *queries.Find("fallback_host");
										}

									}
								}else if(obj->TryGetStringField("signatureCipher", signature))
								{
									requiresDecryption = true;

									FString cipher = obj->GetStringField("signatureCipher");
									TMap<FString, FString> queries = ParseQueryString(cipher);

									if (queries.Contains("s") || queries.Contains("signature"))
									{
										requiresDecryption = queries.Contains("s");
										if (queries.Contains("s"))
										{
											signature = *queries.Find("s");
										}
										else
										{
											signature = *queries.Find("signature");
										}


										if (queries.Contains("sp"))
											signatureQuery = "sig";
										else
											signatureQuery = "signatures";

										url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

										if (queries.Contains("fallback_host"))
										{
											url += "&fallback_host=" + *queries.Find("fallback_host");
										}

									}
								}
								else 
								{
									url = obj->GetStringField("url");
								}

								url = FGenericPlatformHttp::UrlDecode(url);

								TMap<FString, FString> parameters = ParseQueryString(url);
								if (!parameters.Contains("ratebypass"))
								{
									url += "&ratebypass=yes";
								}

								if(useQuality == false)
								{
									if (!requiresDecryption) OnUrlLoaded.Broadcast(*url);
									else
									{
										encryptedUrl = url;
										DecryptVideoUrl(encryptedUrl,"", htmlVersion);
									}
								}
								else {
									encryptedUrl = url;
								}

								UE_LOG(LogTemp, Warning, TEXT("signature:  -> %s"), *signature);
							}
						}

						if (useQuality) 
						{
							for (int32 index = 0; index < streamMap.Num(); index++)
							{
								TSharedPtr<FJsonObject> obj = streamMap[index]->AsObject();
								
								int32 itag = obj->GetIntegerField("itag");
								if (itag == selectedItag) //video quality.
								{
									
									bool requiresDecryption = false;
									FString url;

									FString signature;
									FString signatureQuery = "sig";
									if (obj->TryGetStringField("cipher", signature))
									{
										requiresDecryption = true;

										FString cipher = obj->GetStringField("cipher");
										TMap<FString, FString> queries = ParseQueryString(cipher);

										if (queries.Contains("s") || queries.Contains("signature"))
										{
											requiresDecryption = queries.Contains("s");
											if (queries.Contains("s"))
											{
												signature = *queries.Find("s");
											}
											else
											{
												signature = *queries.Find("signature");
											}


											if (queries.Contains("sp"))
												signatureQuery = "sig";
											else
												signatureQuery = "signatures";

											url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

											if (queries.Contains("fallback_host"))
											{
												url += "&fallback_host=" + *queries.Find("fallback_host");
											}

										}
									}else if(obj->TryGetStringField("signatureCipher", signature))
									{
										requiresDecryption = true;

										FString cipher = obj->GetStringField("signatureCipher");
										TMap<FString, FString> queries = ParseQueryString(cipher);

										if (queries.Contains("s") || queries.Contains("signature"))
										{
											requiresDecryption = queries.Contains("s");
											if (queries.Contains("s"))
											{
												signature = *queries.Find("s");
											}
											else
											{
												signature = *queries.Find("signature");
											}


											if (queries.Contains("sp"))
												signatureQuery = "sig";
											else
												signatureQuery = "signatures";

											url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;

											if (queries.Contains("fallback_host"))
											{
												url += "&fallback_host=" + *queries.Find("fallback_host");
											}

										}
									}
									else
									{
										url = obj->GetStringField("url");
									}

									url = FGenericPlatformHttp::UrlDecode(url);

									TMap<FString, FString> parameters = ParseQueryString(url);
									if (!parameters.Contains("ratebypass"))
									{
										url += "&ratebypass=yes";
									}
									UE_LOG(LogTemp, Verbose, TEXT("Get second url"));
									if (useQuality)
									{
										if (!requiresDecryption) OnUrlLoadedMultiQuality.Broadcast(*encryptedUrl, *url);
										else
										{
											UE_LOG(LogTemp, Verbose, TEXT("Get second durl"));
											videoEncryptedUrl = url;
											DecryptVideoUrl(encryptedUrl, videoEncryptedUrl, htmlVersion);
										}
									}
								}
							}
						}

						//for (FString s : splitByUrls)
						//{
						//	//UE_LOG(LogTemp, Warning, TEXT("String to parse: %s"), *s);
						//	TMap<FString, FString> queries = ParseQueryString(s);

						//	bool requiresDecryption = false;
						//	FString url;

						//	if (queries.Contains("s") || queries.Contains("signature"))
						//	{
						//		requiresDecryption = queries.Contains("s");
						//		FString signature;
						//		if (queries.Contains("s"))
						//		{
						//			signature = *queries.Find("s");
						//		}
						//		else
						//		{
						//			signature = *queries.Find("signature");
						//		}

						//		FString signatureQuery = "sig";

						//		url = *queries.Find("url") + "&" + signatureQuery + "=" + signature;
						//		if (queries.Contains("fallback_host"))
						//		{
						//			url += "&fallback_host=" + *queries.Find("fallback_host");
						//		}

						//	}
						//	else
						//	{
						//		FString resultURL = *queries.Find("url");
						//		url = *queries.Find("url");
						//	}

						//	url = FGenericPlatformHttp::UrlDecode(url);

						//	UE_LOG(LogTemp, Warning, TEXT("URL: %s"), *url);

						//	TMap<FString, FString> parameters = ParseQueryString(url);

						//	if (!parameters.Contains("ratebypass"))
						//	{
						//		url += "&ratebypass=yes";
						//		//TODO USE THE 18 itag
						//	}

						//	FString itag = *ParseQueryString(url).Find("itag");
						//	if (itag == "18") {
						//		//UE_LOG(LogTemp, Warning, TEXT("ITAG: %s"), *itag);
						//		//UE_LOG(LogTemp, Warning, TEXT("Let's Play: %s"), *url);

						//		if (!requiresDecryption) OnUrlLoaded.Broadcast(*url);
						//		else
						//		{
						//			encryptedUrl = url;
						//			DecryptVideoUrl(encryptedUrl, htmlVersion);
						//		}
						//	}
						//}
					}

				//}
			}
			else {
				OnGetUrlError.Broadcast("Unable to get the website json data");
				//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, TEXT("NEIN"));
			}
		}

		////Create a pointer to hold the json serialized data
		//TSharedPtr<FJsonObject> JsonObject;

		////Create a reader pointer to read the json data
		//TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		////Deserialize the json data given Reader and the actual object to deserialize
		//if (FJsonSerializer::Deserialize(Reader, JsonObject))
		//{
		//	//Get the value of the json object by field name
		//	FString recievedInt = JsonObject->GetStringField("customInt");

		//	//Output it to the engine
		//	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, recievedInt);
		//	OnUrlLoaded.Broadcast("VIDEO QUALQUER");
		//}
	}
	else
	{
		OnGetUrlError.Broadcast("Invalid Response");
	}
}

TArray<TSharedPtr<FJsonValue>> UYoutubePlayer::GetStreamMap(TSharedPtr<FJsonObject> json)
{
	TSharedPtr<FJsonObject> streamingData = json->GetObjectField("streamingData");
	TArray<TSharedPtr<FJsonValue>> streamMap = streamingData->GetArrayField("formats");

	return streamMap;
}

TArray<TSharedPtr<FJsonValue>> UYoutubePlayer::GetAdaptiveStreamMap(TSharedPtr<FJsonObject> json)
{
	TSharedPtr<FJsonObject> streamingData = json->GetObjectField("streamingData");
	TArray<TSharedPtr<FJsonValue>> streamMap = streamingData->GetArrayField("adaptiveFormats");
	
	return streamMap;
}

TMap<FString, FString> UYoutubePlayer::ParseQueryString(FString s)
{
	if (s.Contains("?"))
	{
		int32 start = s.Find("?");
		s.RemoveAt(0, start + 1);
		//UE_LOG(LogTemp, Warning, TEXT("ADDDAAAAP: %s"), *s);
	}
	TMap<FString, FString> dictionary;
	TArray<FString> firstSplit;
	s.ParseIntoArray(firstSplit, TEXT("&"), false);
	
	for (FString firstSplited : firstSplit)
	{
		TArray<FString> secondSplit;
		firstSplited.ParseIntoArray(secondSplit, TEXT("="), false);
		//UE_LOG(LogTemp, Warning, TEXT("Amount: %s"), *firstSplited);
		//UE_LOG(LogTemp, Warning, TEXT("Amount: %i"), secondSplit.Num());
		if (secondSplit.Num() == 2)
		{
			FString unscapedUrl = FGenericPlatformHttp::UrlDecode(secondSplit[1]);
			dictionary.Add(secondSplit[0], unscapedUrl);
		}else if(secondSplit.Num() > 2) //for multiple = simbols in same content.
		{
			int32 c = 0;
			FString correct = "";
			for (FString sec : secondSplit)
			{
				if (c > 0) 
				{
					if (c == 1)
						correct += sec;
					else
						correct += "=" + sec;
				}
				c++;
			}
			FString unscapedUrl = FGenericPlatformHttp::UrlDecode(correct);
			dictionary.Add(secondSplit[0], unscapedUrl);
		}
		else
		{
			dictionary.Add(secondSplit[0], "");
		}
	}

	return dictionary;
}


FString UYoutubePlayer::GetHtml5PlayerVersionForFix(FString _url)
{
	FString js = _url;
	//new system to get the js url, ignoring the old ias sys: "http://s.ytimg.com/yts/jsbin/player_ias-" + htmlVersion + ".js";
	jsUrl = "https://www.youtube.com" + js;

	FString pattern = "player_ias-(.+?).js";
	const FRegexPattern myPattern(pattern);
	FRegexMatcher myMatcher(myPattern, js);
	if (myMatcher.FindNext())
	{
		UE_LOG(LogTemp, Verbose, TEXT("RETURNED JS"));
		return myMatcher.GetCaptureGroup(1);
	}
	else
	{
		pattern = "player_ias(.+?).js";
		const FRegexPattern myPatternNewResult(pattern);
		FRegexMatcher matchNewResult(myPatternNewResult, js);
		if (matchNewResult.FindNext())
		{
			UE_LOG(LogTemp, Verbose, TEXT("RETURNED JS on second level"));
			return matchNewResult.GetCaptureGroup(1);
		}
	}

	return FString();
}

FString UYoutubePlayer::GetHtml5PlayerVersion(TSharedPtr<FJsonObject> jsonObject)
{
	TSharedPtr<FJsonObject> assets = jsonObject->GetObjectField("assets");
	FString js = assets->GetStringField("js");
	//new system to get the js url, ignoring the old ias sys: "http://s.ytimg.com/yts/jsbin/player_ias-" + htmlVersion + ".js";
	jsUrl = "https://www.youtube.com" + js;

	FString pattern = "player_ias-(.+?).js";
	const FRegexPattern myPattern(pattern);
	FRegexMatcher myMatcher(myPattern, js);
	if (myMatcher.FindNext())
	{
		UE_LOG(LogTemp, Verbose, TEXT("RETURNED JS"));
		return myMatcher.GetCaptureGroup(1);
	}
	else 
	{
		pattern = "player_ias(.+?).js";
		const FRegexPattern myPatternNewResult(pattern);
		FRegexMatcher matchNewResult(myPatternNewResult, js);
		if (matchNewResult.FindNext()) 
		{
			UE_LOG(LogTemp, Verbose, TEXT("RETURNED JS on second level"));
			return matchNewResult.GetCaptureGroup(1);
		}
	}

	return FString();
}

void UYoutubePlayer::DecryptVideoUrl(FString actualUrl, FString videoUrl, FString htmlVersion)
{
	TMap<FString, FString> queries = ParseQueryString(actualUrl);
	TMap<FString, FString> queriesVideo = ParseQueryString(videoUrl);
	if (queries.Contains("sig"))
	{
		FString signature = *queries.Find("sig");
		sig = signature;

		if(videoUrl != "")
		{
			signature = *queriesVideo.Find("sig");
			videoSig = signature;
		}
		

		FString temporary = actualUrl;
		temporary = temporary.Replace(TEXT("&sig="), TEXT("|"));
		temporary = temporary.Replace(TEXT("&lsig="), TEXT("|"));
		temporary = temporary.Replace(TEXT("&ratebypass=yes"), TEXT(""));
		TArray<FString> splited;
		temporary.ParseIntoArray(splited, TEXT("|"), false);
		lsig = splited[splited.Num() - 2];
		sig = splited[splited.Num() - 1];
		//FString jsUrl = "http://s.ytimg.com/yts/jsbin/player_ias-" + htmlVersion + ".js";

		if(videoUrl != "")
		{
			temporary = videoUrl;
			temporary = temporary.Replace(TEXT("&sig="), TEXT("|"));
			temporary = temporary.Replace(TEXT("&lsig="), TEXT("|"));
			temporary = temporary.Replace(TEXT("&ratebypass=yes"), TEXT(""));
			TArray<FString> splited2;
			temporary.ParseIntoArray(splited2, TEXT("|"), false);
			videoLSig = splited2[splited2.Num() - 2];
			videoSig = splited2[splited2.Num() - 1];
		}

		//UE_LOG(LogTemp, Verbose, TEXT("URIL: %s"), *videoEncryptedUrl);

		GetJsForExtraction(jsUrl);
	}
}



void UYoutubePlayer::GetJsForExtraction(FString url)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UYoutubePlayer::OnResponseReceivedForJS);
	//This is the url on which to process the request
	Request->SetURL(url);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0 (Chrome)");
	Request->ProcessRequest();
}

void UYoutubePlayer::OnResponseReceivedForJS(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (ResponseIsValid(Response, bWasSuccessful))
	{
		FString js = Response->GetContentAsString();
		(new FAutoDeleteAsyncTask<VideoDecryptionAsyncronous>(js, sig, lsig, encryptedUrl, videoSig, videoLSig, videoEncryptedUrl, this))->StartBackgroundTask();
	}
}

FString UYoutubePlayer::GetFunctionFromLine(FString currentLine)
{
	FString matchFunctionReg = "\\w+.\\(";
	const FRegexPattern myPattern(matchFunctionReg);
	FRegexMatcher myMatcher(myPattern, *currentLine);
	FString res;
	if (myMatcher.FindNext())
	{
		res = myMatcher.GetCaptureGroup(0);
		if (res.Contains("("))
		{
			int32 start = res.Find("(");
			res.RemoveAt(start);
		}
		//UE_LOG(LogTemp, Warning, TEXT("V: %s"), *res);

	}
	return *res;
}

TArray<TCHAR> UYoutubePlayer::ReverseFunction(TArray<TCHAR> a)
{
	Algo::Reverse(a);
	return a;
}

TArray<TCHAR> UYoutubePlayer::SpliceFunction(TArray<TCHAR> a, int32 index)
{
	TArray<TCHAR> newArray;
	//UE_LOG(LogTemp, Verbose, TEXT("Total: %i Val: %i"), a.Num(), index);
	for (int32 idx = 0; idx < a.Num(); idx++)
	{
		if (a[idx] == '\0')
		{

		}
		else
		{
			newArray.Add(a[idx]);
		}
	}

	newArray.RemoveAt(0, index);

	return newArray;
}

TArray<TCHAR> UYoutubePlayer::SwapFunction(TArray<TCHAR> a, int32 b)
{
	if (a[0] == '\0')
	{
		a.RemoveAt(0);
	}
	TCHAR c = a[0];
	a[0] = a[b % a.Num()];
	a[b % a.Num()] = c;
	return a;
}


// Called when the game starts
void UYoutubePlayer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UYoutubePlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

