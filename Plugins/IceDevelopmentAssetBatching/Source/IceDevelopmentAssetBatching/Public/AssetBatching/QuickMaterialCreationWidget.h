// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

/**
 * 
 */
UCLASS()
class ICEDEVELOPMENTASSETBATCHING_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
#pragma region QuickMaterialMainCore
protected:
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CreateMaterialFromSelectedTextures")
	bool bCustomMaterialName = true;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CreateMaterialFromSelectedTextures", meta = (EditCondition = "bCustomMaterialName"))
	FString MaterialName = TEXT("M_");
#pragma endregion 
};
