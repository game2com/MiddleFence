// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MiddleFenceDataTypes.generated.h"


UENUM(BlueprintType)
enum class EActionInputType : uint8
{
	None,
	CIT_RightWeaponLightAttack,
	CIT_RightWeaponLightAttackHold,
	CIT_RightWeaponHeavyAttack,
	CIT_RightWeaponHeavyAttackHold,
	CIT_Delay,
	CIT_LeftWeaponLightAttack,
	CIT_LeftWeaponLightAttackHold,
	CIT_LeftWeaponHeavyAttack,
	CIT_LeftWeaponHeavyAttackHold,
	CIT_Dynamic_LightAttack,
	CIT_Dynamic_LightAttackHold,
	CIT_Dynamic_HeavyAttack,
	CIT_Dynamic_HeavyAttackHold
};

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	None,
	Weapon,
	Consumable
};

UENUM(BlueprintType)
enum class EInventorySlotType : uint8
{
	None,
	RightWeapon,
	LeftWeapon
};



USTRUCT(BlueprintType)
struct FComboInput
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EActionInputType Input = EActionInputType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TimeLimit = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float AddedTime = 0.f;

	// Helper function to check if this input matches another input
	bool Matches(const EActionInputType& OtherInput) const
	{
		return Input == OtherInput;
	}
};


USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
		TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
		TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
		class UMiddleFenceCharAnimDataAsset* CharacterAnimation;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UBlendSpace* MovementBlendspace = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimSequenceBase* IdleAnimation = nullptr;
};

USTRUCT(BlueprintType)
struct FSwordTraceAnimationDataRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FComboInput> ActionInputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ClearActionInputBuffer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> TraceSamplingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TraceSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MotionWarpingDistanceToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FramePauseTimeOnTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HitPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bForceEnableTraceDebug;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<class UCameraShakeBase> Shake_Override;
};

//UCLASS()
//class MIDDLEFENCE_API USwordTraceAnimationDataTable : public UDataTable
//{
//	GENERATED_BODY()
//
//
//};

//UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
//class MIDDLEFENCE_API USwordTraceAnimationData : public UObject
//{
//	GENERATED_BODY()
//
//public:
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector> HandPosition;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector> HandForward;
//
//public:	
//	UFUNCTION(BlueprintCallable)
//	void UpdateHandLocationAndForward(TArray<FVector> Positions, TArray<FVector> Forwards) 
//	{
//		HandPosition.Empty();
//		HandForward.Empty();
//
//		HandPosition = Positions;
//		HandForward = Forwards;
//	}
//};

UCLASS(BlueprintType, NotBlueprintable)
class USwordHitGameplayEventData : public UObject
{
	GENERATED_BODY()

public:

	USwordHitGameplayEventData()
	{
		HitPoint = FVector::ZeroVector;
		HitNormal = FVector::ZeroVector;
	}


	UPROPERTY(BlueprintReadOnly, Category = "MyEventPayload")
		FVector HitPoint;

	UPROPERTY(BlueprintReadOnly, Category = "MyEventPayload")
		FVector HitNormal;

	void SetSwordHitGameplayEventData(FVector hitPoint, FVector hitNormal)
	{
		HitPoint = hitPoint;
		HitNormal = hitNormal;
	}
};


USTRUCT(BlueprintType)
struct FComboChain
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FComboInput> CharacterInputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EInventorySlotType SlotType;

	// Helper function to check if this combo chain matches a given input buffer
	bool Matches(const TArray<FComboInput>& ComboInputBuffer) const
	{
		if (ComboInputBuffer.Num() < CharacterInputs.Num())
		{
			return false; // Input buffer doesn't contain enough inputs to match combo chain
		}

		for (int32 i = 0; i < CharacterInputs.Num(); ++i)
		{
			const FComboInput& theInput = CharacterInputs[i];
			const EActionInputType& comboInputType = GetValidInput(ComboInputBuffer[i].Input);

			if (!theInput.Matches(comboInputType))
			{
				return false; // Input doesn't match this combo input
			}

			if (theInput.TimeLimit > 0.f)
			{
				// Check if the input was executed within the time limit
				//float TimeSinceInput = FPlatformTime::Seconds() - ComboInput.LastChangeTime;
				//if (TimeSinceInput > ComboInput.TimeLimit)
				//{
				//	return false; // Input was executed too late
				//}
			}
		}

		return true; // Input buffer matches this combo chain
	}

	EActionInputType GetValidInput(EActionInputType originalInput) const
	{
		if (SlotType == EInventorySlotType::RightWeapon)
		{
			if (originalInput == EActionInputType::CIT_Dynamic_LightAttack)
			{
				return EActionInputType::CIT_RightWeaponLightAttack;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_HeavyAttack)
			{
				return EActionInputType::CIT_RightWeaponHeavyAttack;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_LightAttackHold)
			{
				return EActionInputType::CIT_RightWeaponLightAttackHold;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_HeavyAttackHold)
			{
				return EActionInputType::CIT_Dynamic_HeavyAttackHold;
			}
		}
		else if (SlotType == EInventorySlotType::LeftWeapon)
		{
			if (originalInput == EActionInputType::CIT_Dynamic_LightAttack)
			{
				return EActionInputType::CIT_LeftWeaponLightAttack;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_HeavyAttack)
			{
				return EActionInputType::CIT_LeftWeaponHeavyAttack;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_LightAttackHold)
			{
				return EActionInputType::CIT_LeftWeaponLightAttackHold;
			}
			else if (originalInput == EActionInputType::CIT_Dynamic_HeavyAttackHold)
			{
				return EActionInputType::CIT_LeftWeaponHeavyAttackHold;
			}
		}

		return originalInput;
	}
};


USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EInventorySlotType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMiddleFenceInventoryBase* ItemInstance;
};