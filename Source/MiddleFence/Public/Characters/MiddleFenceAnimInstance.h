// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MiddleFenceAnimInstance.generated.h"


/************************************************************************/
/* UMiddleFenceAnimInstance                                             */
/************************************************************************/
UCLASS(Config = Game)
class MIDDLEFENCE_API UMiddleFenceAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		class AMiddleFenceAdvanceCharacter* AdvanceCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		float MaxMoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		float ForwardValue = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		float LeftValue = 0;


	/************************************************************************/
	/* DefaultAnimations                                                    */
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	class UMiddleFenceCharAnimDataAsset* DefaultAnimations;


public:

	/************************************************************************/
	/* NativeInitializeAnimation                                            */
	/************************************************************************/
	void NativeInitializeAnimation() override;

	/************************************************************************/
	/* NativeUpdateAnimation                                                */
	/************************************************************************/
	void NativeUpdateAnimation(float DeltaSeconds) override;

	/************************************************************************/
	/* GetAdvanceCharacter                                                  */
	/************************************************************************/
	class AMiddleFenceAdvanceCharacter* GetAdvanceCharacter();

protected:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "ModularAnimation", meta = (BlueprintThreadSafe))
		class UBlendSpace* GetBasicMovementRunAnimBlend() const;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "ModularAnimation", meta = (BlueprintThreadSafe))
		class UAnimSequenceBase* GetBasicMovementIdleAnim() const;
};
