// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORIZONSTC_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = HUD)
		void UpdateQuestText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = HUD)
		void SetCrosshairEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = HUD)
		void ToggleCrosshair();

	UPROPERTY(BlueprintReadWrite, Category = HUD, meta = (BindWidget))
		class UTextBlock* ObjTextBlock;

	UPROPERTY(BlueprintReadWrite, Category = HUD, meta = (BindWidget))
		UUserWidget* CrosshairWidget;

	bool bCrosshairEnabled;
};
