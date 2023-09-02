// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseControlsObjectRotatio.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ICECOMMANDLIB_API UMouseControlsObjectRotatio : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMouseControlsObjectRotatio();
	UFUNCTION(BlueprintCallable,Category = "MouseControlsObjectRotation")
	void SetMouseDragRotate(UStaticMeshComponent* target, const float MouseX, const float MouseY);
	
};
