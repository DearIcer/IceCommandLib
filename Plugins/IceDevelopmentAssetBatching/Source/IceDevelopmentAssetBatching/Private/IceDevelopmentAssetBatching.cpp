// Copyright Epic Games, Inc. All Rights Reserved.

#include "IceDevelopmentAssetBatching.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Textures/SlateIcon.h"
#include "IceDevelopmentAssetBatching/IceDebugHeader.h"
#include "IceDevelopmentAssetBatching/Public/AdvancedOptions/AdvancedOptionsWiget.h"

#define LOCTEXT_NAMESPACE "FIceDevelopmentAssetBatchingModule"

void FIceDevelopmentAssetBatchingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	InitCBMenuExtention();
	RegisterAdvanceDeletionTab();
}

void FIceDevelopmentAssetBatchingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvancedOptions"));
}

#pragma region MenuExtention

void FIceDevelopmentAssetBatchingModule::InitCBMenuExtention()
{
	// 获取 ContentBrowser 模块的引用
	FContentBrowserModule& ContentBrowserModule =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	// 获取 ContentBrowser 模块的所有扩展菜单项
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders =
		ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// 创建一个自定义菜单项扩展代理
	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::
		CreateRaw(this, &FIceDevelopmentAssetBatchingModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FIceDevelopmentAssetBatchingModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	// 创建一个扩展对象
	TSharedRef<FExtender> MenuExtender(new FExtender);
	// 如果选中的文件夹不为空，则添加一个菜单扩展
	if (SelectedPaths.Num() > 0)
	{
		// 向菜单扩展中添加一个菜单条目
		MenuExtender->AddMenuExtension(FName("Delete"),// 扩展到 "Delete" 菜单后面
			EExtensionHook::After, // 在菜单末尾添加扩展
			TSharedPtr<FUICommandList>(),// 不使用命令列表
			FMenuExtensionDelegate::CreateRaw(this, &FIceDevelopmentAssetBatchingModule::AddCBMenuEntry) // 绑定动作
		);
		FolderPathsSelected = SelectedPaths;
	}
	return MenuExtender;
}

void FIceDevelopmentAssetBatchingModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	// 向菜单构建器添加一个菜单条目
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("---------啊冰资产批处理工具集选项---------")),
		FText::FromString(TEXT("遇到Bug记得告诉大冰！")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FIceDevelopmentAssetBatchingModule::NullFun)
	);
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("删除未使用资产")),
		FText::FromString(TEXT("这个操作会将当前文件夹下的未使用资产删除！")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FIceDevelopmentAssetBatchingModule::OnDeleteUnsuedAssetButtonClicked)
	);
	MenuBuilder.AddMenuEntry
	(
	FText::FromString(TEXT("删除空文件夹")),
	FText::FromString(TEXT("这个操作会删除所有空文件夹")),
	FSlateIcon(),
	FExecuteAction::CreateRaw(this,&FIceDevelopmentAssetBatchingModule::OnDeleteEmptyFolder)
	);
	MenuBuilder.AddMenuEntry
	(
	FText::FromString(TEXT("高级选项")),
	FText::FromString(TEXT("这里可操作性更高！")),
	FSlateIcon(),
	FExecuteAction::CreateRaw(this,&FIceDevelopmentAssetBatchingModule::OnAdvanceOptionsButtonClicked)
	);
	MenuBuilder.AddMenuEntry
	(
	FText::FromString(TEXT("---------啊冰资产批处理工具集选项---------")),
	FText::FromString(TEXT("遇到Bug记得告诉大冰！")),
	FSlateIcon(),
	FExecuteAction::CreateRaw(this, &FIceDevelopmentAssetBatchingModule::NullFun)
	);
}

#pragma endregion

#pragma region MenuExtentionEvent

void FIceDevelopmentAssetBatchingModule::NullFun()
{
}

void FIceDevelopmentAssetBatchingModule::OnDeleteUnsuedAssetButtonClicked()
{
	// 如果选中的文件夹数量大于1，则提示用户只能选择一个文件夹进行操作，并返回
	if (FolderPathsSelected.Num() > 1)
	{
		DebugHeader::ShowMesDialog(EAppMsgType::Ok,TEXT("只能选一个文件夹进行操作！"));
		return;
	}
	// 获取选中文件夹内的所有资产路径和名称
	TArray<FString> AssetsPathName = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	// 如果文件夹为空，则提示用户并返回
	if (AssetsPathName.Num() == 0)
	{
		DebugHeader::ShowMesDialog(EAppMsgType::Ok,TEXT("文件夹为空！"));
		return;
	}
	// 弹出确认对话框，询问用户是否删除未使用的资产
	EAppReturnType::Type ConfirmResult =
		DebugHeader::ShowMesDialog(EAppMsgType::YesNo,TEXT("一共有 ")
			+ FString::FromInt(AssetsPathName.Num()) + TEXT(" 个资产！是否继续？"));
	// 如果用户选择“否”，则返回
	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}
	// 修复引用
	FixUpRedirectors();
	// 定义一个数组，用于保存未使用的资产数据
	TArray<FAssetData>  UnusedAssetsDataArray;
	// 遍历选中文件夹内的所有资产，检查其是否未被使用，并将未被使用的资产数据保存到数组中
	for (const FString& AssetPathName : AssetsPathName)
	{
		// 过滤引擎内容
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}
		// 如果资产不存在，则跳过此资产
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}
		// 查找引用此资产的资产数量，如果为0，则将此资产添加到未使用的资产数据数组中
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}
	// 如果未使用的资产数据数组不为空，则删除其中的所有资产
	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		// 如果未使用的资产数据数组为空，则提示用户未找到未使用资产
		DebugHeader::ShowMesDialog(EAppMsgType::Ok,TEXT("未找到未使用资产！"));
	}
}

void FIceDevelopmentAssetBatchingModule::OnDeleteEmptyFolder()
{
	FixUpRedirectors();
	// 列出所选文件夹中的所有资产
	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0],true,true);
	int32 Counter = 0;
	// 用于存储空文件夹的路径名和路径名数组
	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFolderPathsNamesArray;
	for(const FString& FolderPath : FolderPathsArray)
	{
		if (FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("Collections")))
		{
			continue;
		}
		if(!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
		{
			continue;
		}
		// 如果文件夹不包含资产，则将其添加到空文件夹路径名和路径名数组中
		if(!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));
			EmptyFolderPathsNamesArray.Add(FolderPath);
		}
	}
	if (EmptyFolderPathsNamesArray.Num() == 0)
	{
		DebugHeader::ShowNotifyinfo(TEXT("选中文件夹不为空！"));
		return;;
	}
	EAppReturnType::Type ConfirmResult =
		DebugHeader::ShowMesDialog(EAppMsgType::OkCancel,
			TEXT("空文件夹有: \n") + EmptyFolderPathsNames + TEXT("\n 你真的要删除它吗?"),
			false);
	if (ConfirmResult == EAppReturnType::Cancel)
	{
		return;
	}
	// 对于每个空文件夹，尝试删除它
	for (const FString& EmptyFolderPath : EmptyFolderPathsNamesArray)
	{
		UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath)
		?
			++Counter : DebugHeader::ShowNotifyinfo(TEXT("删除失败") + EmptyFolderPath);
	}
	if (Counter > 0)
	{
		DebugHeader::ShowNotifyinfo(TEXT("删除成功") + FString::FromInt(Counter) + TEXT("个文件夹"));
	}
}

void FIceDevelopmentAssetBatchingModule::OnAdvanceOptionsButtonClicked()
{
	//DebugHeader::PrintMessage(TEXT("测试信息"),FColor::Green);
	FixUpRedirectors();
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedOptions"));
}

#pragma endregion


#pragma region CustomEditorTab

void FIceDevelopmentAssetBatchingModule::RegisterAdvanceDeletionTab()
{
	// 注册名为“AdvancedOptions”的全局选项卡
	// 创建一个TabSpawner，并将其注册到全局TabManager中
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvancedOptions"),
		FOnSpawnTab::CreateRaw(this,&FIceDevelopmentAssetBatchingModule::OnSpawnAdvanceDeltionTab))
		.SetDisplayName(FText::FromString(TEXT("高级选项")))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

TSharedRef<SDockTab> FIceDevelopmentAssetBatchingModule::OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (GetAllAssetDataSelectedFolder().Num() == 0)
	{
		return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				// Put your tab content here!
				SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("上次编辑器关闭异常，如果需要此工具，请关闭当前页面重新打开！")))
				]
			];
	}
	return
	// 创建一个SDockTab对象，并添加一个名为SAdvanceDeletionTab的SWidget作为其子项
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SAdvancedOptionsWiget)
			.AssetsDataArray(GetAllAssetDataSelectedFolder())
			.CurrentSelectdFolder(FolderPathsSelected[0])
	];
}

#pragma endregion

#pragma region OtherEvent

void FIceDevelopmentAssetBatchingModule::FixUpRedirectors()
{
	// 创建一个指向 UObjectRedirector 对象的 TArray 数组
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	//如果模块已经加载，则返回实例
	// 使用 FAssetRegistryModule 类，我们可以从 AssetRegistry 模块中检索到现有的资产
	FAssetRegistryModule& AssetRegistryModule =
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	//拿到模块后，可以对容器里的对象进行控制

	//设置过滤器
	// 创建一个 Asset Registry Filter，以获取所有的 ObjectRedirector 资产
	FARFilter Filter;
	Filter.bRecursivePaths = true;// 遍历所有子文件夹
	Filter.PackagePaths.Emplace("/Game");// 只检查 /Game 文件夹下的资源
	Filter.ClassNames.Emplace("ObjectRedirector");// 只检查 ObjectRedirector 类型的资源
	
	// 使用 AssetRegistry 模块获取所有符合 Filter 的资产数据
	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);

	// 遍历符合 Filter 的资产数据，如果是 ObjectRedirector 类型的资产，就添加到 RedirectorsToFixArray 数组中
	for (const FAssetData & RedirectorsData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorsData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}

		// 加载 AssetTools 模块，使用其 FixupReferencers 函数来修复所有指向 RedirectorsToFixArray 中的 ObjectRedirector 资产的引用
		FAssetToolsModule & AssetToolsModule =
			FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
	}
}

TArray<TSharedPtr<FAssetData>> FIceDevelopmentAssetBatchingModule::GetAllAssetDataSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvaiableAssetsData;
	if (FolderPathsSelected.Num() == 0)
	{
		return TArray<TSharedPtr<FAssetData>>();
	}
	TArray<FString> AssetsPathName = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	for (const FString& AssetPathName : AssetsPathName)
	{
		// 过滤引擎内容
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}
		// 如果资产不存在，则跳过此资产
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}
		// 查找引用此资产的资产数量，如果为0，则将此资产添加到未使用的资产数据数组中
		// 后续需求，新增开关是否加入有引用资产
		//TArray<FString>AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		//if (AssetReferencers.Num() == 0)
		//{
		
		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvaiableAssetsData.Add(MakeShared<FAssetData>(Data));
		//}
	}

	return AvaiableAssetsData;
}

#pragma endregion

#pragma region ProccessDataForAssetList

bool FIceDevelopmentAssetBatchingModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	// 首先将该对象添加到一个数组 AssetDataForDeletion 中
	TArray<FAssetData> AssetDataForDeletion;
	AssetDataForDeletion.Add(AssetDataToDelete);

	// 然后调用 ObjectTools::DeleteAssets 函数，传入 AssetDataForDeletion 数组，执行删除操作。
	// 如果返回值大于 0，则表示删除成功，返回 true；否则表示删除失败，返回 false。
	if (ObjectTools::DeleteAssets(AssetDataForDeletion) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FIceDevelopmentAssetBatchingModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData> AssetDatasToDelete)
{
	// 调用 ObjectTools::DeleteAssets 函数，传入 AssetDatasToDelete 数组，执行删除操作。
	// 如果返回值大于 0，则表示删除成功，返回 true；否则表示删除失败，返回 false。
	if (ObjectTools::DeleteAssets(AssetDatasToDelete) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void FIceDevelopmentAssetBatchingModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnseDatas)
{
	// 首先清空未使用的资产数据数组
	OutUnseDatas.Empty();

	// 然后遍历待过滤的资产数据数组 AssetDataToFilter
	for (const TSharedPtr<FAssetData>& Data : AssetDataToFilter)
	{
		// 调用函数获取该资产的包引用器数组
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(Data->ObjectPath.ToString());

		// 如果该资产没有被任何一个包引用，将其加入未使用的资产数据数组 OutUnseDatas 中
		if (AssetReferencers.Num() == 0)
		{
			OutUnseDatas.Add(Data);
		}
	}
}

void FIceDevelopmentAssetBatchingModule::ListSameNameAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutDatas)
{
	OutDatas.Empty(); // 清空输出数组,确保操作唯一性

	TMultiMap<FString,TSharedPtr<FAssetData>> AssetsInfoMultiMap; // 创建一个字符串到 FAssetData 智能指针的多重映射表

	// 遍历输入的 FAssetData 智能指针数组，将每个 FAssetData 按照其 AssetName 字符串添加到多重映射表中对应的列表中
	for (const TSharedPtr<FAssetData> & DataSharedPtr : AssetDataToFilter)
	{
		AssetsInfoMultiMap.Emplace(DataSharedPtr->AssetName.ToString(),DataSharedPtr);
	}
	
	// 再次遍历输入的 FAssetData 智能指针数组
	for (const TSharedPtr<FAssetData> & DataSharedPtr : AssetDataToFilter)
	{
		TArray<TSharedPtr<FAssetData>> OutAssetsData;
		AssetsInfoMultiMap.MultiFind(DataSharedPtr->AssetName.ToString(),OutAssetsData); // 从多重映射表中查找所有具有相同 AssetName 字符串的 FAssetData

		if (OutAssetsData.Num() <= 1) // 如果只找到了一个，说明这个 FAssetData 是唯一的，无需处理
		{
			continue;
		}

		// 否则将所有的相同 FAssetData 加入输出数组中（除非该 FAssetData 的智能指针无效）
		for (const TSharedPtr<FAssetData> & SameNameDataDataSharedPtr : OutAssetsData)
		{
			if (SameNameDataDataSharedPtr.IsValid())
			{
				OutDatas.AddUnique(SameNameDataDataSharedPtr);
			}
		}
	}
}

void FIceDevelopmentAssetBatchingModule::SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync)
{
	//要同步的数据
	TArray<FString> AssetsPathToSync;
	AssetsPathToSync.Add(AssetPathToSync);
	//同步数据，接受的是Tarray
	UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
}
#pragma endregion
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FIceDevelopmentAssetBatchingModule, IceDevelopmentAssetBatching)