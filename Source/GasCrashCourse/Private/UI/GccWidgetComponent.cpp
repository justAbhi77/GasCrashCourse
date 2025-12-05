// Copyright. All Rights Reserved.


#include "UI/GccWidgetComponent.h"
#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "Characters/GccBaseCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "UI/GccAttributeWidget.h"


UGccWidgetComponent::UGccWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGccWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemData();

	if(!IsAscInitialized())
	{
		CrashCharacter->OnAscInitialized.AddDynamic(this, &ThisClass::OnAscInitialized);
		return;
	}

	InitializeAttributeDelegate();
}

void UGccWidgetComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UGccWidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<AGccBaseCharacter>(GetOwner());
	AttributeSet = Cast<UGccAttributeSet>(CrashCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UGccAbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());
}

bool UGccWidgetComponent::IsAscInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UGccWidgetComponent::InitializeAttributeDelegate()
{
	if(!AttributeSet->bAttributesInitialized)
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	else
		BindToAttributeChanges();
}

void UGccWidgetComponent::OnAscInitialized(UAbilitySystemComponent* Asc, UAttributeSet* As)
{
	AbilitySystemComponent = Cast<UGccAbilitySystemComponent>(Asc);
	AttributeSet = Cast<UGccAttributeSet>(As);

	// Check if the Attribute Set has been initialized with the first GE
	// If not, bind to some delegate that will be broadcast when it is initialized.

	if(!IsAscInitialized()) return;
	InitializeAttributeDelegate();
}

void UGccWidgetComponent::BindToAttributeChanges()
{
	// Listen for changes to Gameplay Attributes and update our widgets accordingly.
	for(const auto& Pair : AttributeMap)
	{
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair); // for checking the owned widget object.

		GetUserWidgetObject()->WidgetTree->ForEachWidget([this, &Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	}
}

void UGccWidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UGccAttributeWidget* AttributeWidget = Cast<UGccAttributeWidget>(WidgetObject);
	if(!IsValid(AttributeWidget)) return; // We only care about CC Attribute Widgets
	if(!AttributeWidget->MatchesAttributes(Pair)) return; // Only subscribe for matching Attributes

	AttributeWidget->AvatarActor = CrashCharacter;

	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), 0.f); // for initial values.

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), AttributeChangeData.OldValue); // For attribute value changes during the game.
	});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), AttributeChangeData.OldValue); // For attribute max value changes during the game.
	});
}
