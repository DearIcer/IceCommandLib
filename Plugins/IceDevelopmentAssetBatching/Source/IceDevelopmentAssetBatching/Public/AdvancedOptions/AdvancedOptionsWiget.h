// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Widgets/Views/STableRow.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"

// 定义了一个名为SAdvanceDeletionTab的Slate组件类
class SAdvancedOptionsWiget : public SCompoundWidget
{
	// SLATE_BEGIN_ARGS 和 SLATE_END_ARGS 宏用于在类定义中声明Slate组件的构造参数
	SLATE_BEGIN_ARGS(SAdvancedOptionsWiget){}

	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>,AssetsDataArray)

	SLATE_ARGUMENT(FString,CurrentSelectdFolder)
	SLATE_END_ARGS()

public:
	
	/**
	 * @brief 该函数用于构建SAdvancedOptionsWiget组件的UI布局
	 * @param InArgs 
	 */
	void Construct(const FArguments& InArgs);

private:
#pragma region AssetData
	/**
	 * @brief 是否将选择资产同步到内容浏览器
	 */
	TSharedPtr<bool>  bSynchronizeTheSelectedItemToTheContentBrowser;
	/**
	 * @brief 选中文件夹的资产数据
	 */
	TArray<TSharedPtr<FAssetData>> AssetDatasSelectedFolderArray;

	/**
	 * @brief 要删除的资产数据
	 */
	TArray<TSharedPtr<FAssetData>> AssetsDataToDeleteArray;

	/**
	 * @brief 显示的资产数据
	 */
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;

#pragma endregion

#pragma region SlateWiget
	
	/**
	 * @brief 构造资源显示列表
	 * @return 
	 */
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	
	/**
	 * @brief 生成列表数据
	 * @param AssetDataToDisplay 
	 * @param OwnerTable 
	 * @return 
	 */
	TSharedRef<ITableRow>OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,const TSharedRef<STableViewBase> &OwnerTable);
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;

	/**
	 * @brief 构造多选框
	 * @param AssetDataToDisplay 
	 * @return 
	 */
	TSharedRef<SCheckBox>ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TArray<TSharedRef<SCheckBox>> CheckBoxesArray;

	/**
	 * @brief 构造行文本
	 * @param TextContent 
	 * @param FontInfo 
	 * @return 
	 */
	TSharedRef<STextBlock>ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontInfo);

	/**
	 * @brief 按钮构造器
	 * @param AssetDataToDisplay 
	 * @return 
	 */
	TSharedRef<SButton>ConstructButton(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	
	/**
	 * @brief 删除选中按钮
	 * @return 
	 */
	TSharedRef<SButton>ConstructDeleteAllButton();
	
	/**
	 * @brief 选择全部按钮
	 * @return 
	 */
	TSharedRef<SButton>ConstructSelectedButton();
	
	/**
	 * @brief 取消选择按钮
	 * @return 
	 */
	TSharedRef<SButton>ConstructDelectedButton();

	/**
	 * @brief 为Tab按钮构造文本
	 * @param Content 
	 * @return 
	 */
	TSharedRef<STextBlock> ConstructTextForTabButton(const FString& Content);

	/**
	 * @brief 刷新资产列表
	 * @return 
	 */
	TSharedRef<SButton>ConstructRefreshAssetListViewButton();

	/**
	 * @brief 构造多选框
	 * @return 
	 */
	TSharedRef<SCheckBox>ConstructSynchronousOrNotCheckBox(const TSharedPtr<bool>& bSync);
	
#pragma endregion

#pragma region ComboBoxForListingCondition
	
	/**
	 * @brief 构造组合框
	 * @return 
	 */
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();

	TArray<TSharedPtr<FString>> ComboxSourceItems;

	/**
	 * @brief 组合框数据
	 * @param SourceItem 
	 * @return 
	 */
	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem);

	/**
	 * @brief 组合框内容修改事件
	 * @param SelectedOption 
	 * @param InSelectInfo 
	 */
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption,ESelectInfo::Type InSelectInfo);

	TSharedPtr<STextBlock> ComboDisplayTextBlock;

	/**
	 * @brief 构造组合框帮助文本
	 * @return 
	 */
	TSharedRef<STextBlock> ConstructComHelpTexts(const FString & TextConten, ETextJustify::Type TextJustify);
#pragma endregion

#pragma region Ohter
	
	FSlateFontInfo GetEmboseedTextFont() const
	{
		return FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	}
	
	/**
	 * @brief 刷新资产列表
	 */
	void RefreshAssetListView();
#pragma endregion

#pragma region SlateWigetEvent
	
	/**
	 * @brief 多选框状态修改
	 * @param NewState 
	 * @param AssetData 
	 */
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	/**
	 * @brief 删除按钮点击事件
	 * @param ClickedAssetData 
	 * @return 
	 */
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	/**
	 * @brief 删除全部按钮点击事件
	 * @return 
	 */
	FReply OnDeleteAllButtonClicked();

	/**
	 * @brief 取消全部多选框选择
	 * @return 
	 */
	FReply OnDelectedButtonClicked();

	/**
	 * @brief 选择全部多选框
	 * @return 
	 */
	FReply OnSelectedButtonClicked();

	/**
	 * @brief 刷新资产列表
	 * @return 
	 */
	FReply ConstructRefreshAssetListViewButtonClicked();

	/**
	 * @brief 是否同步内容浏览器点击事件
	 * @param NewState 选择状态
	 * @param bSync 同步状态
	 * @return 
	 */
	void OnSynchronousOrNotCheckBoxStateChanged(ECheckBoxState NewState,TSharedPtr<bool> bSync);

	/**
 	 * @brief 返回选中状态
 	 * @param bSync 
 	 * @return 
 	 */
 	static  ECheckBoxState GetSynchronousOrNotCheckBoxState(TSharedPtr<bool> bSync);

	/**
	 * @brief 列表数据点击事件
	 * @param ClickedData 
	 */
	void OnAssetListViewMouseButtonClick(TSharedPtr<FAssetData> ClickedData);
#pragma endregion 
};