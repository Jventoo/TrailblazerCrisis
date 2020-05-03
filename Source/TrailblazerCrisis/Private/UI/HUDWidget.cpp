// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "UI/HUDWidget.h"
#include "Components/TextBlock.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCrosshairEnabled = false;
}

void UHUDWidget::UpdateQuestText(const FText& Text)
{
	ObjTextBlock->SetText(Text);
}

void UHUDWidget::SetCrosshairEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
		bCrosshairEnabled = true;
	}
	else
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
		bCrosshairEnabled = false;
	}
}

void UHUDWidget::ToggleCrosshair()
{
	if (bCrosshairEnabled)
		SetCrosshairEnabled(false);
	else
		SetCrosshairEnabled(true);
}