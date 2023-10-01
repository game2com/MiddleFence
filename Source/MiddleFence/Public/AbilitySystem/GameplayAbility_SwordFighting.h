// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MiddleFenceGameplayAbilityBase.h"
#include "GameplayAbility_SwordFighting.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UGameplayAbility_SwordFighting : public UMiddleFenceGameplayAbilityBase
{
	GENERATED_BODY()
	
protected:

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = nullptr;
	UAbilityTask_WaitGameplayEvent* WaitEventTask = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WaitForEventTag_DealDamage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> SwordDamageEffect;

	UPROPERTY(EditDefaultsOnly)
	FName AnimationWarpTargetName;

	UPROPERTY(EditDefaultsOnly)
	float AnimationPlaySpeed = 1.0f;

	class AInventoryItem_Sword* InstigatorSword;


	virtual void K2_EndAbility() override;

public:
	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION(BlueprintCallable)
	void EventReceived(FGameplayEventData Payload);
};
