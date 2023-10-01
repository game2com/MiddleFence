// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MiddleFenceAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "Characters/MiddleFenceCharMovementComponent.h"

void UMiddleFenceAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		AActor* SourceActor = Data.EffectSpec.GetEffectContext().GetInstigator();

		OnHealthChange.Broadcast(GetHealth(), GetMaxHealth(), SourceActor);

		
		//if (SourceActor != nullptr)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("%s SourceActor"), *GetNameSafe(SourceActor));
		//	// The actor that triggered the effect is SourceActor
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("SourceActor is null"));
		//}
		//UE_LOG(LogTemp, Warning, TEXT("%s Took Damage, New Health : %f"), *GetNameSafe(GetOwningActor()), GetHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute())
	{
		AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwningActor());
		if (advanceCharacter)
		{
			UMiddleFenceCharMovementComponent* movementComponent = Cast<UMiddleFenceCharMovementComponent>(advanceCharacter->GetCharacterMovement());
			if (movementComponent)
			{
				movementComponent->MaxWalkSpeed = GetMaxMovementSpeed();
			}
		}
	}
}
