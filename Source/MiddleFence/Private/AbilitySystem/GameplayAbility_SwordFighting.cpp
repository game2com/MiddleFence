// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbility_SwordFighting.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/TargetHelperComponent.h"
#include "MotionWarpingComponent.h"
#include "Inventory/InventoryItem_Sword.h"


void UGameplayAbility_SwordFighting::K2_EndAbility()
{
	if (InstigatorSword)
	{
		InstigatorSword->OnSwordAnimationEnd();
	}

	Super::K2_EndAbility();
}

bool UGameplayAbility_SwordFighting::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, nullptr, nullptr, nullptr))
		return false;


	return true;
}

void UGameplayAbility_SwordFighting::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}
	}

	UAnimMontage* CurrentMontageToPlay = nullptr;
	if (TriggerEventData->Instigator)
	{
		AInventoryItem_Sword* sword = Cast<AInventoryItem_Sword>(TriggerEventData->Instigator);
		if (sword)
		{
			InstigatorSword = sword;
			CurrentMontageToPlay = sword->GetCurrentComboAnimMontage();
		}
	}

	AMiddleFenceAdvanceCharacter* character = Cast<AMiddleFenceAdvanceCharacter>(ActorInfo->AvatarActor.Get());
	if (character)
	{
		character->UpdateMotionWarpingOnTarget(AnimationWarpTargetName, InstigatorSword ? InstigatorSword->GetWarpMotionDistanceFromTarget() : 100);
		//if (AnimationWarpTargetName.IsNone() == false)
		//{
		//	FVector WarpTargetPos = character->GetActorLocation();
		//	FRotator WarpTargetRot = character->GetActorRotation();

		//	if (character->TargetHelper->GetCurrentTarget())
		//	{
		//		FVector TargetLoc = character->TargetHelper->GetCurrentTarget()->GetActorLocation();
		//		FVector Direction = TargetLoc - WarpTargetPos;
		//		Direction.Normalize();

		//		float Offset = InstigatorSword ? InstigatorSword->GetWarpMotionDistanceFromTarget() : 100;
		//		WarpTargetPos = TargetLoc - (Direction * Offset);
		//		WarpTargetRot = Direction.Rotation();
		//		character->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(AnimationWarpTargetName, WarpTargetPos, WarpTargetRot);
		//	}
		//	else
		//	{
		//		character->GetMotionWarpingComponent()->RemoveWarpTarget(AnimationWarpTargetName);
		//	}
		//}
	}



	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CurrentMontageToPlay, AnimationPlaySpeed);

	PlayMontageTask->OnBlendOut.AddDynamic(this, &UGameplayAbility_SwordFighting::K2_EndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UGameplayAbility_SwordFighting::K2_EndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UGameplayAbility_SwordFighting::K2_EndAbility);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UGameplayAbility_SwordFighting::K2_EndAbility);

	PlayMontageTask->ReadyForActivation();


	WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WaitForEventTag_DealDamage, nullptr, false, true);
	WaitEventTask->EventReceived.AddDynamic(this, &UGameplayAbility_SwordFighting::EventReceived);
	WaitEventTask->ReadyForActivation();


	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGameplayAbility_SwordFighting::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(PlayMontageTask))
	{
		PlayMontageTask->EndTask();
	}

	if (IsValid(WaitEventTask))
	{
		WaitEventTask->EndTask();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_SwordFighting::EventReceived(FGameplayEventData Payload)
{
	if (SwordDamageEffect)
	{
		AActor* target = const_cast<AActor*>(Payload.Target.Get());
		FGameplayAbilityTargetDataHandle targetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(target);

		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, targetData, SwordDamageEffect, 1);


		if (Payload.OptionalObject->IsValidLowLevel())
		{
			const USwordHitGameplayEventData* HitEventData = Cast<USwordHitGameplayEventData>(Payload.OptionalObject.Get());
			if (HitEventData)
			{

			}
		}
	}
}

