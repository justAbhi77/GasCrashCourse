// Copyright Epic Games, Inc. All Rights Reserved.

#include "Utils/DebugUtil.h"

// Log categories
DEFINE_LOG_CATEGORY(LogGcc);
DEFINE_LOG_CATEGORY(LogOthers);

// Console variables acting as runtime debug toggles
static TAutoConsoleVariable CVarDebugGcc(TEXT("Gcc.Debug"), 1, TEXT("Master Debug Toggle for Gas Crash Course"));
static TAutoConsoleVariable CVarDebugPrintToScreen(TEXT("Gcc.Debug.PrintToScreen"), 1, TEXT("Enable/Disable Print To Screen"));
static TAutoConsoleVariable CVarDebugPrintToLog(TEXT("Gcc.Debug.PrintToLog"), 1, TEXT("Enable/Disable Print To Log"));
static TAutoConsoleVariable CVarDebugOthers(TEXT("Gcc.Debug.Others"), 1, TEXT("Enable other debug prints"));


// Timestamp (Real Clock Time)
FString UDebugUtil::BuildTimestamp()
{
	const FDateTime Now = FDateTime::Now();
	return Now.ToString(TEXT("%H:%M:%S.%ms"));
}

// Category-Based Debug Color
FColor UDebugUtil::GetColorForCategory(const EDebugCategories Category, const FLinearColor Default)
{
	switch(Category)
	{
		case EDebugCategories::Edc_Gcc : return FColor::Yellow;

		case EDebugCategories::Edc_Others:
		default:
			return Default.ToFColor(false);
	}
}

// Category Filtering Based on Console Vars
bool UDebugUtil::IsCategoryEnabled(const EDebugCategories Category)
{
	// Master toggle
	if(CVarDebugGcc.GetValueOnAnyThread() == 0)
		return false;

	switch(Category)
	{
		case EDebugCategories::Edc_Gcc : return true;

		case EDebugCategories::Edc_Others : return CVarDebugOthers.GetValueOnAnyThread() != 0;

		default:
			return false;
	}
}

// Unified Debug Message Printer
void UDebugUtil::PrintDebugMessage(const FString& Msg, const EDebugCategories Category, const FLinearColor LinearColor, const float Time, const char* FunctionName, const int Line, const bool bWarning)
{
#if !UE_BUILD_SHIPPING

	if(!IsCategoryEnabled(Category))
		return;

	// Build all metadata
	const FString Timestamp = BuildTimestamp();
	const FString FunctionStr = ANSI_TO_TCHAR(FunctionName);
	const FString EnumVal = UEnum::GetValueAsString(Category);

	// Final formatted output
	const FString FinalMsg = FString::Printf(TEXT("[%s][%s][%s:%d] %s"), *Timestamp, *EnumVal, *FunctionStr, Line, *Msg);

	// Log to Output Log
	if(CVarDebugPrintToLog.GetValueOnAnyThread() != 0 || bWarning)
	{
		switch(Category)
		{
			case EDebugCategories::Edc_Gcc :
				UE_LOG(LogGcc, Log, TEXT("%s"), *FinalMsg);
				break;

			case EDebugCategories::Edc_Others :
			default:
				UE_LOG(LogOthers, Log, TEXT("%s"), *FinalMsg);
				break;
		}
	}

	// Print on screen
	if((CVarDebugPrintToScreen.GetValueOnAnyThread() != 0 || bWarning) && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, Time, GetColorForCategory(Category, LinearColor), FinalMsg);

#endif
}

// Blueprint Wrapper
void UDebugUtil::PrintDebugMessageBP(const FString& Msg, const FString& Function, const int Line, const EDebugCategories Category, const FLinearColor Color, const float Time)
{
	PrintDebugMessage(Msg, Category, Color, Time, TCHAR_TO_ANSI(*Function), Line);
}
