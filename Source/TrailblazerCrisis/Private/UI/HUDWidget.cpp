// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "UI/HUDWidget.h"
#include "Components/TextBlock.h"

void UHUDWidget::UpdateQuestText(const FText& Text)
{
	ObjTextBlock->SetText(Text);
}