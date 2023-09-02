// 这个组件是鼠标控制物体旋转

#include "MouseControlsObjectRotation/MouseControlsObjectRotatio.h"

UMouseControlsObjectRotatio::UMouseControlsObjectRotatio()
{
	PrimaryComponentTick.bCanEverTick = true;
}



void UMouseControlsObjectRotatio::SetMouseDragRotate(UStaticMeshComponent* target, const float MouseX,
	const float MouseY)
{
	target->AddWorldRotation(FRotator(-MouseY,-MouseX,0));
}


