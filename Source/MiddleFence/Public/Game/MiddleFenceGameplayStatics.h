// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "MiddleFenceGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UMiddleFenceGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()
	
public:

	/*
	* How To Use :
	* TablePattern = RowName,Param01,Param02,...
	* Inputs[1] = Row01,1,2,...
	* Inputs[2] = Row02,3,4,...
	*/
	UFUNCTION(BlueprintPure, Category = "AnimationModifier")
	static FString ConvertStringToCSVFormat(FString TablePattern, TArray<FString> Inputs);


	UFUNCTION(BlueprintCallable, Category = "AnimationHelper")
	static FTransform GetBoneTransformFromMontage(const USkeletalMeshComponent* MeshComponent, const UAnimMontage* anim_montage, FName target_bone_name, float Time);

	UFUNCTION(BlueprintCallable, Category = "AnimationHelper")
	static FTransform GetBoneTransformFromSequence(const USkeletalMeshComponent* MeshComponent, UAnimSequence* anim_sequence, FName target_bone_name, float Time, bool WorldSpace);

	UFUNCTION(BlueprintPure, Category = "MathHelper")
		static float GetNegativeAngleBetweenCameraPointAndTarget2D(AActor* Target, APlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "MathHelper")
		static float GetAbsoluteAngleBetweenCameraPointAndTarget2D(AActor* Target, APlayerController* PlayerController);
};
