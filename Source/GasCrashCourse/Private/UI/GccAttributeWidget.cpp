// Copyright. All Rights Reserved.


#include "UI/GccAttributeWidget.h"

void UGccAttributeWidget::OnAttributeChange(const UGccAttributeSet* AttributeSet, const float OldValue)
{
	const float AttributeValue = Attribute.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = MaxAttribute.GetNumericValue(AttributeSet);

	BP_OnAttributeChange(AttributeValue, MaxAttributeValue, OldValue);
}

void UGccAttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, const UGccAttributeSet* AttributeSet, const float OldValue)
{
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);

	BP_OnAttributeChange(AttributeValue, MaxAttributeValue, OldValue);
}

bool UGccAttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}
