// Fill out your copyright notice in the Description page of Project Settings.


#include "Touch/TouchController.h"

void ATouchController::BeginPlay()
{
	Super::BeginPlay();
}

bool ATouchController::FingerIsValid(int32 index)
{
	return FingerIndexs.IsValidIndex(index);
}

bool ATouchController::AddFingerLocation(const ETouchIndex::Type& touchIndex, const FVector& inVector)
{
	if (FingerIndexs.Add(touchIndex) == Locations.Add(inVector))
	{
		return true;
	}
	else
	{
		FingerIndexs.Remove(touchIndex);
		Locations.Remove(inVector);
		return false;
	}
}

void ATouchController::IsFirstOneFingerTouch(bool& ReutrnValue, FVector& touchVector)
{
	ReutrnValue = ((FingerIndexs.Num() == 1) && FingerIsValid(0) && (FingerIndexs[0] == ETouchIndex::Touch1));
	touchVector = Locations[0];
}

void ATouchController::IsFirstTwoFingersTouch(bool& ReutrnValue)
{
	ReutrnValue = (FingerIndexs.Num() == 2);
}

float ATouchController::GetTwoFingerDistance()
{
	if (FingerIsValid(0) && FingerIsValid(1))
	{
		FVector2D index0(Locations[0].X, Locations[0].Y);
		FVector2D index1(Locations[1].X, Locations[1].Y);
		return FVector2D::Distance(index0, index1);
	}
	return 0;
}

void ATouchController::UpdateFingerLocation(const ETouchIndex::Type& index, const FVector& inVector)
{
	if (FingerIsValid(index))
	{
		if (Locations.IsValidIndex(FingerIndexs.Find(index)))
		{
			Locations[FingerIndexs.Find(index)] = inVector;
		}
	}
}

void ATouchController::GetTouchMoveXLength(float& Distance)
{
	if (FingerIsValid(0))
	{
		Distance = Locations[0].X - FixedTouchPoint.X;
	}
}

void ATouchController::GetTouchMoveYLength(float& Distance)
{
	if (FingerIsValid(0))
	{
		Distance = Locations[0].Y - FixedTouchPoint.Y;
	}
}

void ATouchController::GetTwoFingerMoveLength(float& Distance)
{
	Distance = GetTwoFingerDistance() - FixedTwoFingerDistance;
}

bool ATouchController::RemoveFingerLocation(const ETouchIndex::Type& index)
{
	if (FingerIndexs.Contains(index))
	{
		if (Locations.IsValidIndex(FingerIndexs.Find(index)))
		{
			Locations.RemoveAt(FingerIndexs.Find(index));
			FingerIndexs.RemoveAt(FingerIndexs.Find(index));
		}
		return true;
	}
	return false;
}

void ATouchController::ReleasedTouch()
{
	if (FingerIndexs.Num() == 0)
	{
		FixedTouchPoint = FVector(0, 0, 0);
		OperateType = EOperationType::Default;
	}
}
