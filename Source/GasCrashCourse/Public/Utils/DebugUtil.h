// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugUtil.generated.h"

// Log categories exposed to UE_LOG()
DECLARE_LOG_CATEGORY_EXTERN(LogGcc, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogOthers, Log, All);

// Blueprint-exposed enum of our debug domains
UENUM(BlueprintType)
enum class EDebugCategories : uint8
{
	Edc_Gcc		UMETA(DisplayName = "Gcc"),
	Edc_Others  UMETA(DisplayName = "Others")
};

UCLASS()
class GASCRASHCOURSE_API UDebugUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Builds a human-readable timestamp such as 14:55:12.123
	static FString BuildTimestamp();

	// Returns the color associated with each debug category
	static FColor GetColorForCategory(EDebugCategories Category);

	// Checks if a specific debug category is currently enabled via console vars
	static bool IsCategoryEnabled(EDebugCategories Category);

	/**
	 * Central debug print function for logs + screen output.
	 * @param Msg Message to print
	 * @param Category Debug category
	 * @param LinearColor Message color for on-screen log
	 * @param Time How long message stays on screen
	 * @param FunctionName __FUNCTION__ injected automatically
	 * @param Line __LINE__ injected automatically
	 */
	static void PrintDebugMessage(const FString& Msg, EDebugCategories Category = EDebugCategories::Edc_Others, const FLinearColor LinearColor = FLinearColor::White, float Time = 5.f, const char* FunctionName = "", int Line = 0);

	// Blueprint-accessible version of PrintDebugMessage
	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void PrintDebugMessageBP(const FString& Msg, const FString& Function, const int Line, EDebugCategories Category = EDebugCategories::Edc_Others, const FLinearColor Color = FLinearColor::White, float Time = 5.f);
};

// Convenience macros for C++
#define PRINT_DEBUG(msg) \
	UDebugUtil::PrintDebugMessage(msg, EDebugCategories::Edc_Others, FLinearColor::Cyan, 5.f, __FUNCTION__, __LINE__);

// Convenience macros for C++
#define PRINT_DEBUG_MESSAGE(msg, category, color, time) \
	UDebugUtil::PrintDebugMessage(msg, category, color, time, __FUNCTION__, __LINE__);

