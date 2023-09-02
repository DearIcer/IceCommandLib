// Fill out your copyright notice in the Description page of Project Settings.


#include "Touch/TouchPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Touch/TouchController.h"

// Sets default values
ATouchPawn::ATouchPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//添加新的component
	SceneCameraComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneCamera"));
	SceneCameraComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SceneCameraComp->SetRelativeRotation(FRotator(-40.0f, 0, 0));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->AttachToComponent(SceneCameraComp, FAttachmentTransformRules::KeepRelativeTransform);
	SpringArmComp->TargetArmLength = 1200.0f;
	SpringArmComp->bDoCollisionTest = false;


	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CapsuleComp->SetCapsuleHalfHeight(88.0f);
	CapsuleComp->SetCapsuleRadius(44.0f);
}

// Called when the game starts or when spawned
void ATouchPawn::BeginPlay()
{
	Super::BeginPlay();
	InitArmLength = SpringArmComp->TargetArmLength;
}

// Called every frame
void ATouchPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATouchPawn::GetRealPawnDetail(FStateDetail& PawnDetail)
{
	PawnDetail.ActorRotation = GetActorRotation();
	PawnDetail.CameraRotation = SceneCameraComp->GetRelativeRotation();
	PawnDetail.ArmLength = SpringArmComp->TargetArmLength;
	PawnDetail.ActorLocation = GetActorLocation();
}

void ATouchPawn::Turn(float XLength, FRotator ActorTouchedRota, float Factor)
{
	//X轴移动距离  × 系数 × 摄像机X视角度数 + 触摸时Actor.Yaw角度
	float Yaw = XLength * CameraComp->FieldOfView * Factor + ActorTouchedRota.Yaw;
	SetActorRotation(FRotator(0, Yaw, 0));
}

void ATouchPawn::LookUp(float YLength, FRotator CamTouchedRota, float Factor, FVector2D Limit)
{
	if (Limit.X != 0)
	{
		//Y轴移动距离  × 系数 × 摄像机Y视角度数 + 触摸时SceneCameraComp.pitch角度
		float Pitch = YLength * Factor * (CameraComp->FieldOfView / CameraComp->AspectRatio) + CamTouchedRota.Pitch;
		float ClampPitch = FMath::Clamp<float>(Pitch, Limit.X, Limit.Y);
		SceneCameraComp->SetRelativeRotation(FRotator(ClampPitch, 0, 0));
	}
}

void ATouchPawn::Moving(FVector TouchedLocation, float TouchedLength, float XLength, float YLength, float Factor,
	FVector4 Limit)
{
	if (Limit.X != 0) 
	{
		//前后移动
		FVector forwardMove = GetActorForwardVector() * YLength * Factor * (TouchedLength / InitArmLength);
		//左右移动
		FVector rightMove = GetActorRightVector() * XLength * (-1) * Factor * (TouchedLength / InitArmLength);
		FVector newPos = TouchedLocation + forwardMove + rightMove;
		FVector ClampOffsetPos = FVector(FMath::Clamp<float>(newPos.X, Limit.W, Limit.X), FMath::Clamp<float>(newPos.Y, Limit.Y, Limit.Z), newPos.Z);
		SetActorLocation(ClampOffsetPos);
	}
}

void ATouchPawn::Zoom(float TwoFingerDistance, float TouchedLength, float Factor, FVector2D Limit)
{
	if (Limit.X != 0)
	{
		//双指实时距离 × 系数 × (触摸时摄像机高度 ÷ 摄像机原始高度) + 触摸时摄像机高度
		float Length = TwoFingerDistance * Factor * (TouchedLength / InitArmLength) + TouchedLength;
		float ClampLen = FMath::Clamp<float>(Length, Limit.X, Limit.Y);
		SpringArmComp->TargetArmLength = ClampLen;
	}
}

// Called to bind functionality to input
void ATouchPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

