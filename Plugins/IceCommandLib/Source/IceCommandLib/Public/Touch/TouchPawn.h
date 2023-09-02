// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TouchPawn.generated.h"

UCLASS()
class ICECOMMANDLIB_API ATouchPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATouchPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Component")
	class USceneComponent* SceneCameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Component")
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Component")
	class UCameraComponent* CameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Component")
	class UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Variables")
	float InitArmLength;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Blueprintpure, Category = "Move|Function")
	void GetRealPawnDetail(FStateDetail& PawnDetail);
	UFUNCTION(BlueprintCallable, Category = "Move|Function")
	void Turn(float XLength, FRotator ActorTouchedRota, float Factor);
	UFUNCTION(BlueprintCallable, Category = "Move|Function")
	void LookUp(float YLength, FRotator CamTouchedRota, float Factor, FVector2D Limit);
	UFUNCTION(BlueprintCallable, Category = "Move|Function")
	void Moving(FVector TouchedLocation, float TouchedLength, float XLength, float YLength, float Factor, FVector4 Limit);
	UFUNCTION(BlueprintCallable, Category = "Move|Function")
	void Zoom(float TwoFingerDistance, float TouchedLength, float Factor, FVector2D Limit);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
