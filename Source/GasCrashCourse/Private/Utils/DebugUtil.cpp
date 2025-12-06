// Copyright Epic Games, Inc. All Rights Reserved.

#include "Utils/DebugUtil.h"

// Log Categories
DEFINE_LOG_CATEGORY(LogGcc);
DEFINE_LOG_CATEGORY(LogOthers);

// Console Variables
static TAutoConsoleVariable CVarDebugGcc(TEXT("Gcc.Debug"), 1, TEXT("Master Debug Toggle For Gas Crash Course"), ECVF_Default);

static TAutoConsoleVariable CVarDebugPrintToScreen(TEXT("Gcc.Debug.PrintToScreen"), 1, TEXT("Enable/Disable Print To Screen"), ECVF_Default);
static TAutoConsoleVariable CVarDebugPrintToLog(TEXT("Gcc.Debug.PrintToLog"), 1, TEXT("Enable/Disable Print To Log"), ECVF_Default);
static TAutoConsoleVariable CVarDebugOthers(TEXT("Gcc.Debug.Others"), 1, TEXT("Any other debug toggle"), ECVF_Default);


// Timestamp (Real Clock Time - Option B)

FString UDebugUtil::BuildTimestamp()
{
	const FDateTime Now = FDateTime::Now();
	return Now.ToString(TEXT("%H:%M:%S.%ms"));
}

// Category Color
FColor UDebugUtil::GetColorForCategory(const EDebugCategories Category)
{
	switch(Category)
	{
		case EDebugCategories::Edc_Gcc : return FColor::Cyan;

		case EDebugCategories::Edc_Others : return FColor::White;

		default: return FColor::White;
	}
}

// Category Filter
bool UDebugUtil::IsCategoryEnabled(const EDebugCategories Category)
{
	if(CVarDebugGcc.GetValueOnAnyThread() == 0) return false;

	switch(Category)
	{
		case EDebugCategories::Edc_Gcc : return true;

		case EDebugCategories::Edc_Others: return CVarDebugOthers.GetValueOnAnyThread() > 0;
	}
	return false;
}

// Unified Debug Printer
void UDebugUtil::PrintDebugMessage(const FString& Msg, const EDebugCategories Category, const FLinearColor LinearColor, const float Time, const char* FunctionName, const int Line)
{
#if !UE_BUILD_SHIPPING

	if(!IsCategoryEnabled(Category))
		return;

	// Build prefix
	const FString Timestamp = BuildTimestamp();

	// Extract class/function from PrettyFunction: Example "void AMyActor::FooBar()"
	const FString FunctionStr = ANSI_TO_TCHAR(FunctionName);
	const FString EnumVal = UEnum::GetValueAsString(Category);

	const FString FinalMsg = FString::Printf(TEXT("[%s][%s][%s at %d] %s"), *Timestamp, *EnumVal, *FunctionStr, Line, *Msg);

	// Output Log
	if(!CVarDebugPrintToLog.GetValueOnAnyThread() == 0)
		switch(Category)
		{
			case EDebugCategories::Edc_Gcc:
				UE_LOG(LogGcc, Log, TEXT("%s"), *FinalMsg);
				break;

			case EDebugCategories::Edc_Others :
				UE_LOG(LogOthers, Log, TEXT("%s"), *FinalMsg);
				break;
		}

	// On-Screen Message
	if(!CVarDebugPrintToScreen.GetValueOnAnyThread() == 0 && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, Time, LinearColor.ToFColor(false), FinalMsg);

#endif
}

void UDebugUtil::PrintDebugMessageBP(const FString& Msg, const FString& Function, const int Line, const EDebugCategories Category, const FLinearColor Color, const float Time)
{
    PrintDebugMessage(Msg, Category, Color, Time, TCHAR_TO_ANSI(*Function), Line);
}
