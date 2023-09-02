// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

class FIceDevelopmentAssetBatchingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * @brief 
	 * @return 返回文件夹里的所有资产
	 */
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataSelectedFolder();
private:

#pragma region MenuExtention

	/**
	 * @brief 选中文件夹的路径
	 */
	TArray<FString> FolderPathsSelected;
	
	/**
	 * @brief 生成内容浏览器文件夹右键扩展菜单，在模块加载时调用
	 */
	void InitCBMenuExtention();
	
	/**
	 * @brief 创建扩展对象
	 * @param SelectedPaths 这个是选中的文件夹路径
	 * @return 
	 */
	TSharedRef<FExtender>CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	/**
	 * @brief 创建菜单项
	 * @param MenuBuilder 
	 */
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

#pragma endregion

#pragma region MenuExtentionEvent
	
	/**
	 * @brief 这是个空函数，不进行任何操作
	 */
	void NullFun();
	
	/**
	 * @brief 删除未使用资产
	 */
	void OnDeleteUnsuedAssetButtonClicked();

	/**
	 * @brief 删除空文件夹
	 */
	void OnDeleteEmptyFolder();

	/**
	 * @brief 高级选项，创建扩展窗口
	 */
	void OnAdvanceOptionsButtonClicked();
#pragma endregion

#pragma region CustomEditorTab
	
	/**
	 * @brief 注册名为“AdvancedOptions”的全局选项卡
	 */
	void RegisterAdvanceDeletionTab();

	/**
	 * @brief 当名为“AdvancedOptions”的选项卡被生成时，将会调用该函数 
	 * @param SpawnTabArgs 
	 * @return 
	 */
	TSharedRef<SDockTab>OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs);

#pragma endregion

#pragma region OtherEvent
	
	/**
	 * @brief 修复资产引用
	 */
	void FixUpRedirectors();


#pragma endregion

#pragma region ProccessDataForAssetList
public:
	/**
	 * @brief 删除列表中的单个资产
	 * @param AssetDataToDelete 
	 * @return 
	 */
	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	
	/**
	 * @brief 删除列表中的多个资产
	 * @param AssetDatasToDelete 
	 * @return 
	 */
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData> AssetDatasToDelete);
	
	/**
	 * @brief 获取列表中的未使用资产
	 * @param AssetDataToFilter 
	 * @param OutUnseDatas 
	 */
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>> & OutUnseDatas);

	/**
	 * @brief 获取类型相同，名字相同的资产
	 * @param AssetDataToFilter 
	 * @param OutUnseDatas 
	 */
	void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>> & OutUnseDatas);

	/**
	 * @brief 同步内容浏览器资产
	 * @param AssetPathToSync 
	 */
	void SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync);
#pragma endregion 
};
