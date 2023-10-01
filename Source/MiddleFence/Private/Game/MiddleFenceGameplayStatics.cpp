// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MiddleFenceGameplayStatics.h"

FString UMiddleFenceGameplayStatics::ConvertStringToCSVFormat(FString TablePattern, TArray<FString> Inputs)
{
	FString FinalString = TablePattern + LINE_TERMINATOR;

	for (FString& string : Inputs)
	{
		FinalString += string;
		FinalString += LINE_TERMINATOR;
	}

	return FinalString;
}

FTransform UMiddleFenceGameplayStatics::GetBoneTransformFromMontage(const USkeletalMeshComponent* MeshComponent, const UAnimMontage* anim_montage, FName target_bone_name, float Time)
{
	UAnimSequence* AnimSequence = Cast<UAnimSequence>(anim_montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].GetAnimReference());
	FTransform Transform = GetBoneTransformFromSequence(MeshComponent, AnimSequence, target_bone_name, Time, true);

	return Transform;
}

FTransform UMiddleFenceGameplayStatics::GetBoneTransformFromSequence(const USkeletalMeshComponent* MeshComponent, UAnimSequence* anim_sequence, FName target_bone_name, float Time, bool WorldSpace)
{
	if (!anim_sequence || !MeshComponent || !MeshComponent->GetSkeletalMeshAsset())
	{
		return FTransform::Identity;
	}

	FName bone_name = target_bone_name;
	FTransform global_transform = FTransform::Identity;

	do
	{
		const int bone_index = MeshComponent->GetBoneIndex(bone_name);
		int32 BoneTrackIndex = MeshComponent->GetSkeletalMeshAsset()->GetSkeleton()->GetRawAnimationTrackIndex(bone_index, anim_sequence);

		FTransform bone_transform;
		anim_sequence->GetBoneTransform(bone_transform, BoneTrackIndex, Time, true);

		global_transform *= bone_transform;
		bone_name = MeshComponent->GetParentBone(bone_name);

	} while (bone_name != NAME_None);

	if(WorldSpace)
	global_transform *= MeshComponent->GetComponentTransform();

	return global_transform;
}

float UMiddleFenceGameplayStatics::GetNegativeAngleBetweenCameraPointAndTarget2D(AActor* Target, APlayerController* PlayerController)
{
	if (!PlayerController || !Target) {
		return 0.0f;
	}

	const FVector PawnLocation = PlayerController->GetPawn()->GetActorLocation();
	const FVector TargetDirection = Target->GetActorLocation() - PawnLocation;
	const FVector TargetDirectionXY = FVector(TargetDirection.X, TargetDirection.Y, 0.0f).GetSafeNormal();

	const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

	const FVector ForwardDirection = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X);
	//const FVector ForwardDirection = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
	const FVector ForwardDirectionXY = FVector(ForwardDirection.X, ForwardDirection.Y, 0.0f).GetSafeNormal();

	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetDirectionXY, ForwardDirectionXY)) );

	// Get cross product between camera forward vector and target direction to check if target is to the left or to the right of the camera
	const FVector CrossProduct = FVector::CrossProduct(ForwardDirectionXY, TargetDirectionXY);
	const float Sign = FMath::Sign(CrossProduct.Z);

	return -Sign * Angle;
}

float UMiddleFenceGameplayStatics::GetAbsoluteAngleBetweenCameraPointAndTarget2D(AActor* Target, APlayerController* PlayerController)
{
	if (!PlayerController || !Target) {
		return 0.0f;
	}

	const FVector PawnLocation = PlayerController->GetPawn()->GetActorLocation();
	const FVector TargetDirection = Target->GetActorLocation() - PawnLocation;
	const FVector TargetDirectionXY = FVector(TargetDirection.X, TargetDirection.Y, 0.0f).GetSafeNormal();

	const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

	const FVector ForwardDirection = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X);
	//const FVector ForwardDirection = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
	const FVector ForwardDirectionXY = FVector(ForwardDirection.X, ForwardDirection.Y, 0.0f).GetSafeNormal();

	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetDirectionXY, ForwardDirectionXY)));

	// Get cross product between camera forward vector and target direction to check if target is to the left or to the right of the camera
	//const FVector CrossProduct = FVector::CrossProduct(ForwardDirectionXY, TargetDirectionXY);
	//const float Sign = FMath::Sign(CrossProduct.Z);

	return  Angle;
}

