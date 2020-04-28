// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = HUD)
		void UpdateQuestText(const FText& Text);

	UPROPERTY(BlueprintReadWrite, Category = HUD, meta = (BindWidget))
		class UTextBlock* ObjTextBlock;
};
