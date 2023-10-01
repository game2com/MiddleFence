// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/MiddleFenceCharacterBase.h"
#include "Components/TargetHelperComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMiddleFenceCharacterBase::AMiddleFenceCharacterBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
 	// Disable call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	TargetHelper = CreateDefaultSubobject<UTargetHelperComponent>(TEXT("Target Helper Component"));
}


void AMiddleFenceCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

#pragma region Movement Functionality

void AMiddleFenceCharacterBase::MoveForward(const FInputActionValue& Value)
{
	//implementation on child class
}

void AMiddleFenceCharacterBase::MoveRight(const FInputActionValue& Value)
{
	//implementation on child class
}

void AMiddleFenceCharacterBase::FreeCharacterRotation()
{
	bUseControllerRotationYaw = false;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void AMiddleFenceCharacterBase::ActiveCharacterStrafeRotation()
{
	bUseControllerRotationYaw = true;
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void AMiddleFenceCharacterBase::ActiveCharacterVelocityRotation()
{
	bUseControllerRotationYaw = false;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

#pragma endregion

#pragma region Helper Function

UAnimMontage* AMiddleFenceCharacterBase::GetCurrentActiveMontage() const
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance && AnimInstance->GetCurrentActiveMontage())
	{
		return AnimInstance->GetCurrentActiveMontage();
	}

	return nullptr;
}

float AMiddleFenceCharacterBase::GetCurrentPlayingMontageTime() const
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	UAnimMontage* ActiveMontage = GetCurrentActiveMontage();

	if (ActiveMontage && AnimInstance)
	{
		return AnimInstance->Montage_GetPosition(ActiveMontage);
	}

	return 0;
}

void AMiddleFenceCharacterBase::PauseCurrentActiveAnimMonatge()
{
	PauseAnimMonatge(GetCurrentActiveMontage());
}

void AMiddleFenceCharacterBase::PauseAnimMonatge(UAnimMontage* MontageToPause)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance && MontageToPause)
	{
		AnimInstance->Montage_Pause(MontageToPause);
	}
}

void AMiddleFenceCharacterBase::ResumeAnimMonatge(UAnimMontage* MontageToResume)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (MontageToResume && AnimInstance)
	{
		//AnimInstance->Montage_SetPosition(AnimMontage, NewPosition);
		AnimInstance->Montage_Resume(MontageToResume);
	}
}

void AMiddleFenceCharacterBase::ResumeCurrentActiveAnimMonatge()
{
	ResumeAnimMonatge(GetCurrentActiveMontage());
}

void AMiddleFenceCharacterBase::SetPositionCurrentActiveAnimMonatge(float NewPosition)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (GetCurrentActiveMontage() && AnimInstance)
	{
		AnimInstance->Montage_SetPosition(GetCurrentActiveMontage(), NewPosition);
	}
}

float AMiddleFenceCharacterBase::GetPositionCurrentActiveAnimMonatge() const
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (GetCurrentActiveMontage() && AnimInstance)
	{
		return AnimInstance->Montage_GetPosition(GetCurrentActiveMontage());
	}

	return 0.0f;
}

#pragma endregion

