// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedOptions/AdvancedOptionsWiget.h"

#include "IceDevelopmentAssetBatching.h"
#include "IceDevelopmentAssetBatching/IceDebugHeader.h"
#include "AssetRegistry/AssetData.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SButton.h"

#define ListAll TEXT("查看所有资源")
#define ListUnused TEXT("查看未使用资源")
#define ListSameName TEXT("查看同名资产")

#pragma region SlateWidget

void SAdvancedOptionsWiget::Construct(const FArguments& InArgs)
{
	// 允许该组件获取焦点
	bCanSupportFocus = true;

	// 定义字体样式，设置字体大小为 30
	FSlateFontInfo TitleTextFont = GetEmboseedTextFont();
	TitleTextFont.Size = 30;

	//传递选中文件里的资产数据
	AssetDatasSelectedFolderArray = InArgs._AssetsDataArray;
	DisplayedAssetsData = AssetDatasSelectedFolderArray;

	//是否同步到内容浏览器
	bSynchronizeTheSelectedItemToTheContentBrowser = MakeShared<bool>(false); // 智能指针，并将其初始化为 false

	CheckBoxesArray.Empty();
	AssetsDataToDeleteArray.Empty();
	ComboxSourceItems.Empty();
	
	ComboxSourceItems.Add(MakeShared<FString>(ListAll));
	ComboxSourceItems.Add(MakeShared<FString>(ListUnused));
	ComboxSourceItems.Add(MakeShared<FString>(ListSameName));
	
	ChildSlot
	[
		// 垂直盒子
		SNew(SVerticalBox)

		// 标题
		+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
					.Text(FText::FromString(TEXT("高级选项")))
					.Font(TitleTextFont)
					.Justification(ETextJustify::Center)
					.ColorAndOpacity(FColor::White)
			]
		// 水平盒子,构造组合框
		+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						ConstructComboBox()
					]
					// 水平盒子,构造刷新按钮
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						ConstructRefreshAssetListViewButton()
					]
					//文件路径
					+SHorizontalBox::Slot()
					.FillWidth(.1f)
					[
						ConstructComHelpTexts(TEXT("选中文件夹：\n") + InArgs._CurrentSelectdFolder,ETextJustify::Center)
					]
			]
		// 水平盒子,构造是否将资产同步到内容浏览器多选框
		+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						ConstructSynchronousOrNotCheckBox(bSynchronizeTheSelectedItemToTheContentBrowser)
					]
					//帮助文本
					+SHorizontalBox::Slot()
					.FillWidth(.6f)
					[
						ConstructComHelpTexts(TEXT("在列表点击资源可以跳转到内容浏览器。"),ETextJustify::Center)
					]
			]
		// 滚动列表,显示资产列
		+SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				ConstructAssetListView()
			]
		// 水平盒子
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				//按钮1,删除全部
				+SHorizontalBox::Slot()
					.FillWidth(10.f)
					.Padding(5.f)
					[ConstructDeleteAllButton()]
				//按钮2,全选
				+ SHorizontalBox::Slot()
					.FillWidth(10.f)
					.Padding(5.f)
					[ConstructSelectedButton()]
				//按钮3,取消全选
				+SHorizontalBox::Slot()
					.FillWidth(10.f)
					.Padding(5.f)
					[ConstructDelectedButton()]
			]
	];
}


#pragma endregion

#pragma region SlateWiget

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedOptionsWiget::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)// 创建列表视图
		.ItemHeight(24.f)
		.ListItemsSource(&DisplayedAssetsData)// 列表项数据源---------------------------------------------------------
		.OnGenerateRow(this, &SAdvancedOptionsWiget::OnGenerateRowForList)// 列表项生成委托
		.OnMouseButtonClick(this,&SAdvancedOptionsWiget::OnAssetListViewMouseButtonClick);

	return ConstructedAssetListView.ToSharedRef();
}

TSharedRef<ITableRow> SAdvancedOptionsWiget::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay->IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}
	// 获取要显示的资产名称
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	const FString DisplayAssetClassName = AssetDataToDisplay->GetClass()->GetName();
	FSlateFontInfo DisplayAssetFont = GetEmboseedTextFont();
	DisplayAssetFont.Size = 10;
	// 使用 SNew 宏创建一个新的 STableRow 对象，模板参数为 TSharedPtr<FAssetData>。
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(SHorizontalBox)
				//1.多选框
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(.05f)
				[
					ConstructCheckBox(AssetDataToDisplay)
				]
			//2.显示的资源类名
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				.AutoWidth()
				//.FillWidth(.2f)
				[
					ConstructTextForRowWidget(DisplayAssetClassName, DisplayAssetFont)
				]
			//3.显示的资产名
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					/*SNew(STextBlock)
						.Text(FText::FromString(DisplayAssetName))*/
					ConstructTextForRowWidget(DisplayAssetName, DisplayAssetFont)
				]
			//4.按钮,删除
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Fill)
				[
					ConstructButton(AssetDataToDisplay)
				]

		];
	// 返回一个 TSharedRef<ITableRow> 对象，其中包含 ListViewRowWidget。
	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAdvancedOptionsWiget::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	// 创建 SCheckBox 控件，并将其赋值给 TSharedRef<SCheckBox> 类型的变量 ConstructedCheckBox
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox) // 指定复选框类型
		.OnCheckStateChanged(this, &SAdvancedOptionsWiget::OnCheckBoxStateChanged, AssetDataToDisplay);

	CheckBoxesArray.Add(ConstructedCheckBox);
	
	return ConstructedCheckBox;// 返回构造的复选框控件
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvancedOptionsWiget::ConstructComboBox()
{
	// 创建一个指向另一个模板类的引用，并构造 ComboBox。
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox =
		SNew(SComboBox<TSharedPtr<FString>>)
			// 以 ComboxSourceItems 数据为选项列表数据源，传递给构造函数。
			.OptionsSource(&ComboxSourceItems)
			// 设置 OnGenerateWidget 回调函数，该回调函数将在每次 ComboBox 列表重新生成时调用。
			.OnGenerateWidget(this,&SAdvancedOptionsWiget::OnGenerateComboContent)
			// 设置 OnSelectionChanged 回调函数，该回调函数将在用户更改选择选项时进行调用。
			.OnSelectionChanged(this,&SAdvancedOptionsWiget::OnComboSelectionChanged)
				[
					// 设置文本和颜色/字体属性等。
					SAssignNew(ComboDisplayTextBlock,STextBlock)
						.Text(FText::FromString(TEXT("列表显示设置")))
				]
			;
	// 返回新构造的 ComboBox 的引用。
	return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvancedOptionsWiget::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	// 创建一个指向 STextBlock 类的引用，并对其进行初始化。
	TSharedRef<STextBlock> ConstructedComboText = SNew(STextBlock)
		// 通过 Get() 函数获取 SourceItem 的当前值字符串，将其转换为 FText 类型。
		.Text(FText::FromString(*SourceItem.Get()));
		// 返回新构造的 STextBlock 实例的引用。
	return ConstructedComboText;
}

void SAdvancedOptionsWiget::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption,
	ESelectInfo::Type InSelectInfo)
{
	// 将 SelectedOption 的当前值转换为 FText 类型，并设置给 ComboDisplayTextBlock 控件实例。
	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get())); 

	// 获取 IceDevelopmentAssetBatchingModule 模块实例，并将其存储在 IceModule 变量中。
	FIceDevelopmentAssetBatchingModule & IceModule = FModuleManager::LoadModuleChecked<FIceDevelopmentAssetBatchingModule>(TEXT("IceDevelopmentAssetBatching"));

	// 根据用户选择更新显示的资产列表。
	if (*SelectedOption.Get() == ListAll)
	{
		// 如果用户选择“列表全部”，则显示所有资产。
		DisplayedAssetsData = AssetDatasSelectedFolderArray;
		RefreshAssetListView();
	}
	else if (*SelectedOption.Get() == ListUnused)
	{
		// 如果用户选择“列表未使用”的选项，则调用 ListUnusedAssetsForAssetList 函数以获取未使用的资产数据，获取成功后刷新资产列表视图。
		IceModule.ListUnusedAssetsForAssetList(AssetDatasSelectedFolderArray,DisplayedAssetsData);
		RefreshAssetListView();
	}
	else if (*SelectedOption.Get() == ListSameName)
	{
		// 如果用户选择“显示同名资产”，则显示所有名字相同类型相同的资产。
		IceModule.ListSameNameAssetsForAssetList(AssetDatasSelectedFolderArray,DisplayedAssetsData);
		RefreshAssetListView();
	}
}

TSharedRef<STextBlock> SAdvancedOptionsWiget::ConstructTextForRowWidget(const FString& TextContent,
	const FSlateFontInfo& FontInfo)
{
	// 创建一个指向 STextBlock 类的引用，并对其进行初始化。
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
		// 将 TextContent 转换为 FText 类型，并将其设置为控件的文本属性。
		.Text(FText::FromString(TextContent))
		// 设置控件的字体属性，传入 FontInfo。
		.Font(FontInfo)
		// 设置控件的颜色和不透明度属性，传入纯白色（FColor::White）。
		.ColorAndOpacity(FColor::White);
	// 返回新构造的 STextBlock 实例的引用。
	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvancedOptionsWiget::ConstructButton(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	// 创建一个指向 SButton 类的引用，并对其进行初始化。
	TSharedRef<SButton> ConstructedButton = SNew(SButton)
		// 设置控件的文本属性为“删除”。
		.Text(FText::FromString(TEXT("删除")))
		// 设置按钮被点击时的回调函数，传入 OnDeleteButtonClicked 函数的指针和 AssetDataToDisplay 参数。
		.OnClicked(this, &SAdvancedOptionsWiget::OnDeleteButtonClicked, AssetDataToDisplay);
	// 返回新构造的 SButton 实例的引用。
	return ConstructedButton;
}

TSharedRef<SButton> SAdvancedOptionsWiget::ConstructDeleteAllButton()
{
	// 创建一个指向 SButton 类的引用，并对其进行初始化。
	TSharedRef<SButton> DeleteAllButton = SNew(SButton)
		// 设置按钮的内边距属性，传入 5.f。
		.ContentPadding(FMargin(5.f))
		// 设置按钮被点击时的回调函数，传入 OnDeleteAllButtonClicked 函数的指针。
		.OnClicked(this, &SAdvancedOptionsWiget::OnDeleteAllButtonClicked);

	// 调用 ConstructTextForTabButton 函数来创建一个用于显示在按钮上的文本块，传入“删除选中”作为参数，并设置为按钮的内容。
	DeleteAllButton->SetContent(ConstructTextForTabButton(TEXT("删除选中")));
	// 返回新构造的 SButton 实例的引用。
	return DeleteAllButton;
}

TSharedRef<SButton> SAdvancedOptionsWiget::ConstructSelectedButton()
{
	// 创建一个指向 SButton 类的引用，并对其进行初始化。
	TSharedRef<SButton> SelectedAllButton = SNew(SButton)
		// 设置按钮的内边距属性，传入 5.f。
		.ContentPadding(FMargin(5.f))
		// 设置按钮被点击时的回调函数，传入 OnSelectedButtonClicked 函数的指针。
		.OnClicked(this, &SAdvancedOptionsWiget::OnSelectedButtonClicked);

	// 调用 ConstructTextForTabButton 函数来创建一个用于显示在按钮上的文本块，传入“选择全部”作为参数，并设置为按钮的内容。
	SelectedAllButton->SetContent(ConstructTextForTabButton(TEXT("选择全部")));
	// 返回新构造的 SButton 实例的引用。
	return SelectedAllButton;
}

TSharedRef<SButton> SAdvancedOptionsWiget::ConstructDelectedButton()
{
	// 创建一个指向 SButton 类的引用，并对其进行初始化。
	TSharedRef<SButton> DelectedAllButton = SNew(SButton)
		// 设置按钮的内边距属性，传入 5.f。
		.ContentPadding(FMargin(5.f))
		// 设置按钮被点击时的回调函数，传入 OnDelectedButtonClicked 函数的指针。
		.OnClicked(this, &SAdvancedOptionsWiget::OnDelectedButtonClicked);

	// 调用 ConstructTextForTabButton 函数来创建一个用于显示在按钮上的文本块，传入“取消选择”作为参数，并设置为按钮的内容。
	DelectedAllButton->SetContent(ConstructTextForTabButton(TEXT("取消选择")));
	// 返回新构造的 SButton 实例的引用。
	return DelectedAllButton;
}

TSharedRef<STextBlock> SAdvancedOptionsWiget::ConstructTextForTabButton(const FString& Content)
{
	// 从 GetEmboseedTextFont 函数中获取一个用于样式化文本的字体
	FSlateFontInfo ButtonTextFont = GetEmboseedTextFont();
	// 设置字体大小为 15 像素
	ButtonTextFont.Size = 15;
	// 创建一个指向 STextBlock 类的引用，并对其进行初始化。
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
		// 通过传入 Content 的值设置文本块的显示内容。
		.Text(FText::FromString(Content))
		// 设置文本块的字体，传入 ButtonTextFont。
		.Font(ButtonTextFont)
		// 设置文本块内文本的水平方向对齐方式（居中）。
		.Justification(ETextJustify::Center);

	// 返回新构造的 STextBlock 实例的引用。
	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvancedOptionsWiget::ConstructRefreshAssetListViewButton()
{
	// 创建一个指向 SButton 类的引用，并对其进行初始化。
	TSharedRef<SButton> RefreshAssetListViewButton = SNew(SButton)
		// 设置按钮的内边距属性，传入 5.f。
		.ContentPadding(FMargin(5.f))
		// 设置按钮被点击时的回调函数，传入 ConstructRefreshAssetListViewButtonClicked 函数的指针。
		.OnClicked(this, &SAdvancedOptionsWiget::ConstructRefreshAssetListViewButtonClicked);

	// 调用 ConstructTextForTabButton 函数来创建一个用于显示在按钮上的文本块，传入“刷新资产列表”作为参数，并设置为按钮的内容。
	RefreshAssetListViewButton->SetContent(ConstructTextForTabButton(TEXT("刷新资产列表")));
	// 返回新构造的 SButton 实例的引用。
	return RefreshAssetListViewButton;
}
TSharedRef<SCheckBox> SAdvancedOptionsWiget::ConstructSynchronousOrNotCheckBox(const TSharedPtr<bool>& bSync)
{
	// 创建 SCheckBox 控件，并将其赋值给 TSharedRef<SCheckBox> 类型的变量 ConstructedCheckBox
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox) // 指定复选框类型
		//.IsChecked_Static(&SAdvancedOptionsWiget::GetSynchronousOrNotCheckBoxState, bSync) // 设置复选框的选中状态
		.OnCheckStateChanged(this, &SAdvancedOptionsWiget::OnSynchronousOrNotCheckBoxStateChanged, bSync) 
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(FMargin(2.0f, 0.0f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("是否将所选资产同步到内容浏览器")))
			]
		];
		
	return ConstructedCheckBox;// 返回构造的复选框控件
}

TSharedRef<STextBlock> SAdvancedOptionsWiget::ConstructComHelpTexts(const FString& TextConten,
	ETextJustify::Type TextJustify)
{
	TSharedRef<STextBlock> ConstructComHelpText =
		SNew(STextBlock)
		.Text(FText::FromString(TextConten))
		.Justification(TextJustify)
		.AutoWrapText(true)
	;
	return ConstructComHelpText;
}

#pragma endregion

#pragma region SlateWigetEvent

void SAdvancedOptionsWiget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	// 根据复选框的新状态进行操作
	switch (NewState)
	{
		// 如果复选框未选中
		case ECheckBoxState::Unchecked:
			// 检查资产数据是否已在待删除资产数组中
			if (AssetsDataToDeleteArray.Contains(AssetData))
			{
				// 移除待删除资产数组中的该资产数据
				AssetsDataToDeleteArray.Remove(AssetData);
			}
		break;
		// 如果复选框被选中
		case ECheckBoxState::Checked:
			// 将该资产数据添加到待删除资产数组中，如果已存在则不重复添加
			AssetsDataToDeleteArray.AddUnique(AssetData);
		break;
		// 如果复选框的状态不确定
		case ECheckBoxState::Undetermined:
			break;
		// 如果出现了新的未知状态
		default:
			break;
	}
}

FReply SAdvancedOptionsWiget::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	// 获取并加载 IceDevelopmentAssetBatching 模块，该模块是用于资产批处理的
	FIceDevelopmentAssetBatchingModule& IceModule = FModuleManager::LoadModuleChecked<FIceDevelopmentAssetBatchingModule>(TEXT("IceDevelopmentAssetBatching"));

	// 向模块传递待处理资产数据，并返回是否成功删除单个资产数据
	const bool bAssetDeleted = IceModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	// 如果成功删除单个资产数据
	if (bAssetDeleted)
	{
		// 刷新列表
		// 从文件夹资产数组和展示资产数组中移除已删除的资产数据
		if (AssetDatasSelectedFolderArray.Contains(ClickedAssetData))
		{
			AssetDatasSelectedFolderArray.Remove(ClickedAssetData);
		}
		if (DisplayedAssetsData.Contains(ClickedAssetData))
		{
			DisplayedAssetsData.Remove(ClickedAssetData);
		}
		RefreshAssetListView();
	}
	else
	{
		// 资产删除失败，显示相应提示信息
		DebugHeader::ShowNotifyinfo(TEXT("资产删除失败"));
	}

	// 返回一个响应状态，表示已对用户的操作进行了处理
	return FReply::Handled();
}

FReply SAdvancedOptionsWiget::OnDeleteAllButtonClicked()
{
	// 如果没有选中需要删除的资产，则显示警告对话框
	if (AssetsDataToDeleteArray.Num() == 0)
	{
		DebugHeader::ShowMesDialog(EAppMsgType::Ok,TEXT("没有资产被选中"));
	}
	// 将 AssetsDataToDeleteArray 数组中的所有 FAssetData 类型的对象添加到 AssetsDataToDelete 数组中
	// AssetsDataToDeleteArray 是一个包含 TSharedPtr<FAssetData> 类型指针的数组
	TArray<FAssetData> AssetsDataToDelete;
	for (const TSharedPtr<FAssetData>&Data : AssetsDataToDeleteArray)
	{
		AssetsDataToDelete.Add(*Data.Get());
	}
	// 获取 FIceDevelopmentAssetBatchingModule 模块，调用 DeleteMultipleAssetsForAssetList() 函数来删除指定的所有资产
	// 并存储删除操作是否成功
	FIceDevelopmentAssetBatchingModule & IceModule = FModuleManager::LoadModuleChecked<FIceDevelopmentAssetBatchingModule>(TEXT("IceDevelopmentAssetBatching"));
	//Pass data for our module to filter
	const bool bAssetsDeleted = IceModule.DeleteMultipleAssetsForAssetList(AssetsDataToDelete);
	if (bAssetsDeleted)
	{
		// 如果删除成功，从 AssetDatasSelectedFolderArray 数组中删除所有已删除的资产
		// AssetDatasSelectedFolderArray 是一个包含 TSharedPtr<FAssetData> 类型指针的数组
		for (const TSharedPtr<FAssetData>& DeletedDate : AssetsDataToDeleteArray)
		{
			// 刷新列表
			// 从文件夹资产数组和展示资产数组中移除已删除的资产数据
			if (AssetDatasSelectedFolderArray.Contains(DeletedDate))
			{
				AssetDatasSelectedFolderArray.Remove(DeletedDate);
			}
			if (DisplayedAssetsData.Contains(DeletedDate))
			{
				DisplayedAssetsData.Remove(DeletedDate);
			}
		}
		RefreshAssetListView();
	}
	// 返回一个 FReply 对象表示事件处理已完成
	return FReply::Handled();
}

FReply SAdvancedOptionsWiget::OnDelectedButtonClicked()
{
	// 如果复选框数组为空，则直接返回响应状态，表示已对用户的操作进行了处理
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Handled();
	}

	// 循环遍历复选框数组中所有的复选框
	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		// 如果复选框是被选中状态，则将其切换到未选中状态
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	// 返回一个响应状态，表示已对用户的操作进行了处理
	return FReply::Handled();
}

FReply SAdvancedOptionsWiget::OnSelectedButtonClicked()
{
	// 如果复选框数组为空，则直接返回响应状态，表示已对用户的操作进行了处理
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Handled();
	}

	// 循环遍历复选框数组中所有的复选框
	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		// 如果复选框是未选中状态，则将其切换为选中状态
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	// 返回一个响应状态，表示已对用户的操作进行了处理
	return FReply::Handled();
}

FReply SAdvancedOptionsWiget::ConstructRefreshAssetListViewButtonClicked()
{
	// 通过 IceDevelopmentAssetBatching 模块获取选中文件夹里的所有资产数据
	FIceDevelopmentAssetBatchingModule & IceModule = FModuleManager::LoadModuleChecked<FIceDevelopmentAssetBatchingModule>(TEXT("IceDevelopmentAssetBatching"));
	AssetDatasSelectedFolderArray = IceModule.GetAllAssetDataSelectedFolder();

	// 将获取到的资产数据数组赋值给展示资产数组
	DisplayedAssetsData = AssetDatasSelectedFolderArray;

	// 调用 RefreshAssetListView() 函数刷新展示列表
	RefreshAssetListView();

	// 返回一个响应状态，表示已对用户的操作进行了处理
	return FReply::Handled();
}
void SAdvancedOptionsWiget::OnSynchronousOrNotCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<bool> bSync)
{
	*bSync = (NewState == ECheckBoxState::Checked); // 将用户选择的状态更新到 bSync 变量中
}

ECheckBoxState SAdvancedOptionsWiget::GetSynchronousOrNotCheckBoxState(TSharedPtr<bool> bSync)
{
	//TSharedPtr<bool> bSyncCopy = bSync;   // 利用别名构造函数创建一个与原始对象共享引用计数的对象
	//DebugHeader::PrintMessage(FString::Printf(TEXT("%d"), *bSyncCopy),FColor::Red);
	return (*bSync) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; // 返回复选框的选中状态
}

void SAdvancedOptionsWiget::OnAssetListViewMouseButtonClick(TSharedPtr<FAssetData> ClickedData)
{
	if (*bSynchronizeTheSelectedItemToTheContentBrowser == true)
	{
		// 调用 IceModule 对象的 SyncCBToClickedAssetForAssetList 函数，
		// 并将 ClickedData->ObjectPath.ToString() 转换为字符串后作为参数传递给该函数。
		// 这个函数将会根据 ClickedData 数据，将 Content Browser 中选定的资产同步到该资产的路径中。
		FIceDevelopmentAssetBatchingModule& IceModule = FModuleManager::LoadModuleChecked<FIceDevelopmentAssetBatchingModule>(TEXT("IceDevelopmentAssetBatching"));
		IceModule.SyncCBToClickedAssetForAssetList(ClickedData->ObjectPath.ToString());
	}
}
#pragma endregion

#pragma region Ohter

void SAdvancedOptionsWiget::RefreshAssetListView()
{
	// 首先清空待删除的资产数据数组 AssetsDataToDeleteArray 和复选框数组 CheckBoxesArray
	AssetsDataToDeleteArray.Empty();
	CheckBoxesArray.Empty();

	// 然后检查 ConstructedAssetListView 是否有效，如果有效，重新构建资产列表视图
	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}
#pragma endregion