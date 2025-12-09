// Copyright. All Rights Reserved.

#include "UI/GccWidgetComponent.h"
#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "Characters/GccBaseCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "UI/GccAttributeWidget.h"
#include "Utils/DebugUtil.h"

void UGccWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemData();

	if(!IsAscInitialized())
	{
		if(CrashCharacter.IsValid())
			CrashCharacter->OnAscInitialized.AddDynamic(this, &ThisClass::OnAscInitialized);
		else
			PRINT_DEBUG_WARNING("Invalid CrashCharacter ");
		return;
	}

	InitializeAttributeDelegate();
}

void UGccWidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<AGccBaseCharacter>(GetOwner());

	if(!CrashCharacter.IsValid())
	{
		PRINT_DEBUG_WARNING("CrashCharacter invalid in InitAbilitySystemData");
		return;
	}

	AttributeSet = Cast<UGccAttributeSet>(CrashCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UGccAbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());

	PRINT_DEBUG("InitAbilitySystemData: ASC=%d AttrSet=%d", AbilitySystemComponent.IsValid(), AttributeSet.IsValid());
}

bool UGccWidgetComponent::IsAscInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UGccWidgetComponent::InitializeAttributeDelegate()
{
	if(!AttributeSet.IsValid())
	{
		PRINT_DEBUG_WARNING("AttributeSet invalid in InitializeAttributeDelegate");
		return;
	}

	if(!AttributeSet->bAttributesInitialized)
	{
		PRINT_DEBUG("Attributes not initialized. Binding to OnAttributesInitialized...");
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		PRINT_DEBUG("Attributes already initialized. Binding now...");
		BindToAttributeChanges();
	}
}

void UGccWidgetComponent::OnAscInitialized(UAbilitySystemComponent* Asc, UAttributeSet* As)
{
	PRINT_DEBUG("ASC initialized callback received");

	AbilitySystemComponent = Cast<UGccAbilitySystemComponent>(Asc);
	AttributeSet = Cast<UGccAttributeSet>(As);

	if(!IsAscInitialized())
	{
		PRINT_DEBUG_WARNING("ASC or AttributeSet invalid after OnAscInitialized");
		return;
	}

	InitializeAttributeDelegate();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGccWidgetComponent::BindToAttributeChanges()
{
	if(!AbilitySystemComponent.IsValid() || !AttributeSet.IsValid())
	{
		PRINT_DEBUG_WARNING("Cannot bind attributes: ASC or AttributeSet invalid");
		return;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	UUserWidget* RootWidget = GetUserWidgetObject();
	if(!IsValid(RootWidget))
	{
		PRINT_DEBUG_WARNING("Root widget invalid");
		return;
	}

	PRINT_DEBUG("Binding widgets to attribute changes...");

	RootWidget->WidgetTree->ForEachWidget([this](UWidget* ChildWidget)
	{
		BindWidgetToAttributeChanges(ChildWidget);
	});

	BindWidgetToAttributeChanges(RootWidget);
}

void UGccWidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject) const
{
	UGccAttributeWidget* GccWidget = Cast<UGccAttributeWidget>(WidgetObject);
	if(!GccWidget)
		return;

	GccWidget->AvatarActor = CrashCharacter.Get();

	// Initial update
	GccWidget->OnAttributeChange(AttributeSet.Get(), 0.f);

	TWeakObjectPtr WeakWidget = GccWidget;
	UGccAttributeSet* AttributeSetPtr = AttributeSet.Get();

	// --- Attribute change binding ---
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GccWidget->Attribute).AddLambda([this, WeakWidget, AttributeSetPtr](const FOnAttributeChangeData& Data){
			if(UGccAttributeWidget* Widget = WeakWidget.Get())
			{
				PRINT_DEBUG("Attribute changed. Was %f New value is %f", Data.OldValue, Data.NewValue);
				Widget->OnAttributeChange(AttributeSetPtr, Data.OldValue);
			}
	});

	// --- Max attribute change binding ---
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GccWidget->MaxAttribute).AddLambda([this, WeakWidget, AttributeSetPtr](const FOnAttributeChangeData& Data){
			if(UGccAttributeWidget* Widget = WeakWidget.Get())
			{
				PRINT_DEBUG("Max Attribute changed. Was %f New value is %f", Data.OldValue, Data.NewValue);
				Widget->OnAttributeChange(AttributeSetPtr, Data.OldValue);
			}
	});
}
