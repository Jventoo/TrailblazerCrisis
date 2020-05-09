// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Create_Curves.generated.h"

USTRUCT(BlueprintType)
struct FAnimCurveCreation
{
	GENERATED_USTRUCT_BODY()

	FAnimCurveCreation()
	{
		FrameNumber = 0;
		CurveValue = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
	int32 FrameNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
	float CurveValue;
};

USTRUCT(BlueprintType)
struct FAnimCurveCreationParams
{
	GENERATED_USTRUCT_BODY()

	FAnimCurveCreationParams()
	{
		CurveName = TEXT("None");
		KeyEachFrame = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		FName CurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		bool KeyEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		TArray<FAnimCurveCreation> Keys;
};

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UCreate_Curves : public UAnimationModifier
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimModifier")
		class UAnimSequence* AnimSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimModifier")
		FAnimCurveCreationParams CurveParams;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		TArray<FAnimCurveCreationParams> CurvesToCreate;
};
