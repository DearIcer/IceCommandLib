// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "Blutility/Classes/AssetActionUtility.h"
#include "Engine/CurveTable.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Blueprint/UserWidget.h"
#include "AssetBatching.generated.h"

/**
 * 
 */
UCLASS()
class ICEDEVELOPMENTASSETBATCHING_API UAssetBatching : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
#pragma region ScriptBatchOperations
	/**
	 * @brief 批量复制资产
	 * @param Num 这个参数是编辑器里传进来的
	 */
	UFUNCTION(CallInEditor,Category = "资产操作")
	void DuplicateAssets(int32 Num);

	/**
	 * @brief 自动添加前缀
	 */
	UFUNCTION(CallInEditor,Category = "资产操作")
	void AddPrefixes();
	
	/**
	 * @brief 移除未使用资产
	 */
	UFUNCTION(CallInEditor,Category = "资产操作")
	void RemoveUnusedAsset();

	UFUNCTION(CallInEditor,Category = "资产操作")
	void ShutUpSrgb();
private:
	//声明一份map，用来匹配要修改的前缀。Map最优解速度最快
	//键值对为：类名 ： 修改的前缀
	const TMap<UClass*,FString>PrefixMap =
	{
		{UBlueprint::StaticClass(),TEXT("BP_")},
		// {UAnimBlueprint::StaticClass(),TEXT("ABP_")},
		{UBlendableInterface::StaticClass(),TEXT("BI_")},
		{UCurveTable::StaticClass(),TEXT("CT_")},
		{UEnum::StaticClass(),TEXT("E_")},
		{UMaterial::StaticClass(),TEXT("M_")},
		{UMaterialInstanceConstant::StaticClass(),TEXT("MI_")},
		{UStaticMesh::StaticClass(),TEXT("SM_")},
		{USkeletalMesh::StaticClass(),TEXT("SK_")},
		{UTexture2D::StaticClass(),TEXT("T_")},
		{UPhysicsAsset::StaticClass(),TEXT("PHYS_")},
		{UPhysicalMaterial::StaticClass(),TEXT("PM_")},
		{UActorComponent::StaticClass(),TEXT("AC_")},
		{UStruct::StaticClass(),TEXT("F_")},
		{UUserWidget::StaticClass(),TEXT("WBP_")}
	};
#pragma endregion
};
