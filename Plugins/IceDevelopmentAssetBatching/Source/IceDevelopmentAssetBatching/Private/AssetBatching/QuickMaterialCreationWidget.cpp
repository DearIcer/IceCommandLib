// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetBatching/QuickMaterialCreationWidget.h"

#include "IceDevelopmentAssetBatching/IceDebugHeader.h"

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHeader::ShowMesDialog(EAppMsgType::Ok,TEXT("请输入有效的材质名！必须以M_开头！"));
			return;
		}
	}
}
