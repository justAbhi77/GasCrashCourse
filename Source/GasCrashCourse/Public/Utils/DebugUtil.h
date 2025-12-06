// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugUtil.generated.h"

// Log Categories
DECLARE_LOG_CATEGORY_EXTERN(LogGcc, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogOthers, Log, All);

// Enum for categories
UENUM(BlueprintType)
enum class EDebugCategories : uint8
{
	Edc_Gcc UMETA(DisplayName = "Gcc"),
	Edc_Others UMETA(DisplayName = "Others")
};

UCLASS()
class GASCRASHCOURSE_API UDebugUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FString BuildTimestamp();

	static FColor GetColorForCategory(EDebugCategories Category);

	static bool IsCategoryEnabled(EDebugCategories Category);

	static void PrintDebugMessage(const FString& Msg, EDebugCategories Category = EDebugCategories::Edc_Others, const FLinearColor LinearColor = FLinearColor::White, float Time = 5.f, const char* FunctionName = "", int Line = 0);

	UFUNCTION(BlueprintCallable, Category="Debug")
	static void PrintDebugMessageBP(const FString& Msg, const FString& Function, const int Line, EDebugCategories Category = EDebugCategories::Edc_Others, const FLinearColor Color = FLinearColor::White, float Time = 5.f);
};

#define PRINT_DEBUG(msg) \
	UDebugUtil::PrintDebugMessage(msg, EDebugCategories::Edc_Others, FLinearColor::Cyan, 5.f, __FUNCTION__, __LINE__);

#define PRINT_DEBUG_MESSAGE(msg, category, color, time) \
	UDebugUtil::PrintDebugMessage(msg, category, color, time, __FUNCTION__, __LINE__);

