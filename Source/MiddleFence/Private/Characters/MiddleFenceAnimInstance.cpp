// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MiddleFenceAnimInstance.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "Characters/MiddleFenceCharAnimDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMiddleFenceAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwningActor()))
	{
		if (UCharacterMovementComponent* MovementComponent = advanceCharacter->GetCharacterMovement())
		{
			MaxMoveSpeed = MovementComponent->MaxWalkSpeed;
		}
	}
}

void UMiddleFenceAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (AMiddleFenceAdvanceCharacter* advanceCharacter = GetAdvanceCharacter())
	{
		Velocity = advanceCharacter->GetVelocity();
		//UE_LOG(LogTemp, Warning, TEXT("Velocity : %f, %f, %f"), Velocity.X, Velocity.Y, Velocity.Z);

		bIsMoving = Velocity.Size() != 0;
		//UE_LOG(LogTemp, Warning, TEXT("bIsMoving : %d"), bIsMoving);

		FVector UnrotateVector = advanceCharacter->GetActorRotation().UnrotateVector(Velocity);
		//UE_LOG(LogTemp, Warning, TEXT("UnrotateVector : %f, %f, %f"), UnrotateVector.X, UnrotateVector.Y, UnrotateVector.Z);

		ForwardValue = UnrotateVector.X / MaxMoveSpeed;
		LeftValue = UnrotateVector.Y / MaxMoveSpeed;

		//UE_LOG(LogTemp, Warning, TEXT("ForwardValue : %f, LeftValue : %f"), ForwardValue, LeftValue);
	}
}


class AMiddleFenceAdvanceCharacter* UMiddleFenceAnimInstance::GetAdvanceCharacter()
{
	if (AdvanceCharacter == nullptr)
		AdvanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwningActor());

	return AdvanceCharacter;
}

UBlendSpace* UMiddleFenceAnimInstance::GetBasicMovementRunAnimBlend() const
{
	if (AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwningActor()))
	{
		FCharacterData CharacterData = advanceCharacter->GetCharacterData();

		return CharacterData.CharacterAnimation->CharacterAnimationData.MovementBlendspace;
	}

	return DefaultAnimations ? DefaultAnimations->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UMiddleFenceAnimInstance::GetBasicMovementIdleAnim() const
{
	if (AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwningActor()))
	{
		FCharacterData CharacterData = advanceCharacter->GetCharacterData();

		if(CharacterData.CharacterAnimation && CharacterData.CharacterAnimation->CharacterAnimationData.IdleAnimation)
			return CharacterData.CharacterAnimation->CharacterAnimationData.IdleAnimation;
	}

	return DefaultAnimations ? DefaultAnimations->CharacterAnimationData.IdleAnimation : nullptr;
}

