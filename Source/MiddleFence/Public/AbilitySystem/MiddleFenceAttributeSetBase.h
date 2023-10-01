// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MiddleFenceAttributeSetBase.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangeDelegate, float, health, float, maxHealth, AActor*&, whoSendDamage);

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UMiddleFenceAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMiddleFenceAttributeSetBase, MaxHealth)

    UPROPERTY(BlueprintReadOnly)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UMiddleFenceAttributeSetBase, Health)   
    
	FOnHealthChangeDelegate OnHealthChange;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxMovementSpeed;
	ATTRIBUTE_ACCESSORS(UMiddleFenceAttributeSetBase, MaxMovementSpeed)

    void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;



};
