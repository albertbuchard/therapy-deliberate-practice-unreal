#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TherapyDPSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="TherapyDP Settings"))
class THERAPYDP_API UTherapyDPSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString BaseUrl = TEXT("http://localhost:8000/");

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString AiMode = TEXT("local_prefer");

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString OpenAiApiKey;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString LocalSttUrl;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString LocalLlmUrl;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString LocalLlmModel;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="API")
    FString DbPath;
};
