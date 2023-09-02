// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TouchController.generated.h"

UENUM(BlueprintType, Category = "XZ|Touch")
enum class EOperationType : uint8 {
	Default UMETA(DisplayName = "Default"),
	TowFinger UMETA(DisplayName = "TowFinger"),
	Move UMETA(DisplayName = "Move"),
	Zoom UMETA(DisplayName = "Zoom"),

	Turn UMETA(DisplayName = "Turn")
};

USTRUCT(BlueprintType, Category = "XZ|Touch")
struct FStateDetail {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "XZ|MyTouch|StateDetail")
	FRotator ActorRotation;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|MyTouch|StateDetail")
	FRotator CameraRotation;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|MyTouch|StateDetail")
	float ArmLength;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|MyTouch|StateDetail")
	FVector ActorLocation;
};

/**
 * 
 */
UCLASS()
class ICECOMMANDLIB_API ATouchController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:

	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	TArray<FVector> Locations;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	TArray<TEnumAsByte<ETouchIndex::Type>> FingerIndexs;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	TArray<FString> MoveTypeNames;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	EOperationType OperateType;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	bool CanControl;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	FStateDetail PawnDetail;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	FVector FixedTouchPoint;
	UPROPERTY(BlueprintReadWrite, Category = "XZ|Touch|Variable")
	float FixedTwoFingerDistance;
public:
	//判断手指是否合法
	UFUNCTION()
	bool FingerIsValid(int32 index);
	//添加触碰手指的位置
	UFUNCTION(BlueprintCallable, Category = "XZ|MyTouch|Function")
	bool AddFingerLocation(const ETouchIndex::Type& touchIndex, const FVector& inVector);
	//是否是一个手指触碰
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	void IsFirstOneFingerTouch(bool& ReutrnValue, FVector& touchVector);
	//是否是两个手指触碰
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	void IsFirstTwoFingersTouch(bool& ReutrnValue);
	//获取两指之间的距离
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	float GetTwoFingerDistance();
	//更新手指间的距离
	UFUNCTION(BlueprintCallable, Category = "XZ|MyTouch|Function")
	void UpdateFingerLocation(const ETouchIndex::Type& index, const FVector& inVector);
	//获取触摸X轴的长度
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	void GetTouchMoveXLength(float& Distance);
	//获取触摸Y轴的长度
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	void GetTouchMoveYLength(float& Distance);
	//获取两个手指移动的距离
	UFUNCTION(BlueprintPure, Category = "XZ|MyTouch|Pure")
	void GetTwoFingerMoveLength(float& Distance);
	//移除手指位置
	UFUNCTION(BlueprintCallable, Category = "XZ|MyTouch|Function")
	bool RemoveFingerLocation(const ETouchIndex::Type& index);
	//松开手指触摸
	UFUNCTION(BlueprintCallable, Category = "XZ|MyTouch|Function")
	void ReleasedTouch();
};
