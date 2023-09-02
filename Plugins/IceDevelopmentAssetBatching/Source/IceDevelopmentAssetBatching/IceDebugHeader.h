#pragma once
#include "Editor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Math/Color.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"


namespace DebugHeader
{
	/**
	 * @brief 打印信息到视口
	 * @param Message 要打印的信息
	 * @param Color 颜色
	 */
	static void PrintMessage(const FString & Message, const FColor &Color = FColor::Green)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,8,Color,Message);
			UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);
		}
	}

	/**
	 * @brief 输出Log，在编辑器和Ide都会有默认类型为警告
	 * @param Message 要输出的信息
	 */
	static void PrintUELog(const FString &Message)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);

	}
	
	/**
	 * @brief 弹出一个对话框
	 * @param MsgType 对话框按钮类型
	 * @param Message 要显示的文本
	 * @param bShowMesAsWarning 是否为警告
	 * @param MesTitle 对话框标题
	 * @return 
	 */
	static EAppReturnType::Type ShowMesDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMesAsWarning = true, FText MesTitle = FText::FromString(TEXT("Warning")))
	{
		if (bShowMesAsWarning)
		{
			return FMessageDialog::Open(MsgType, FText::FromString(Message), &MesTitle);
		}
		else
		{
			return FMessageDialog::Open(MsgType, FText::FromString(Message));
		}
	}


	/**
	 * @brief 弹出一个通知列表
	 * @param Message 通知文本
	 */
	static void ShowNotifyinfo(const FString & Message)
	{
		//定义一个通知列表
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		//使用大字体
		NotifyInfo.bUseLargeFont = true;
		//淡出时间
		NotifyInfo.FadeOutDuration = 8.f;
		//添加到编辑器中，需要一个FSlateNotificationManager管理器
		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}

}
