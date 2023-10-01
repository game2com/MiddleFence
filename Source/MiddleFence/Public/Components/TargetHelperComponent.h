// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetHelperComponent.generated.h"

UENUM(BlueprintType)
enum class ETargetSelectionType : uint8
{
	TST_DistanceOnly,
	TST_AngleOnly,
	TST_Both
};

UCLASS( ClassGroup=(Custom), HideCategories = (Collision, Cooking, Sockets, Tags, AssetUserData, ComponentTick, Variable), meta = (BlueprintSpawnableComponent))
class MIDDLEFENCE_API UTargetHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UTargetHelperComponent();

protected:

	/************************************************************************/
	/* BeginPlay															*/
	/************************************************************************/
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
	TSubclassOf<AActor> DefaultSearchClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		bool bSearchByPlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		float DefaultSearchRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		TEnumAsByte<ECollisionChannel> DefaultSearchChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		ETargetSelectionType DefaultTargetSelectionMethod;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		FVector2D DefaultAngleAcceptRange;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Settings")
		bool bCheckTargetHealth;

	/************************************************************************/
	/* Current active target is store here									*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Result")
		AActor* CurrentTarget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Result")
		bool LostTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Result", meta = (EditCondition = "LostTarget", EditConditionHides))
		float LostDistance;


	FTimerHandle TH_LostTargetTimer;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
		bool bDrawTrace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
		FColor TraceDrawColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
		float DrawliveTime = 5.0f;

	/************************************************************************/
	/* FindActorsWithinRadius                                               */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Search")
	TArray<AActor*> FindActorsWithinRadius() const;

	/************************************************************************/
	/* SelectTarget                                                         */
	/************************************************************************/
	//UFUNCTION(BlueprintPure, Category = "Search")
	AActor* SelectTarget(TArray<AActor*> AllActors) const;


public:
	/************************************************************************/
	/* Main function for finding then setting a target						*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Search")
	bool FindAndAssignTarget(); 

	UFUNCTION(BlueprintPure, Category = "TargetHelper")
	AActor* GetCurrentTarget() const;

	UFUNCTION(BlueprintCallable, Category = "TargetHelper")
	void ClearCurrentTarget();

protected:
	/************************************************************************/
	/* KeepTrackOfTarget                                                    */
	/************************************************************************/
	void KeepTrackOfTarget();


	/************************************************************************/
	/* OnLostTarget                                                         */
	/************************************************************************/
	void OnLostTarget();

	/*Call From Outside of the component*/
	//void OnTargetDestroy();

	//void OnTargetDeath();

#pragma region Helper

public:

	/************************************************************************/
	/* GetAbsoluteAngle                                                     */
	/************************************************************************/
	float GetAbsoluteAngle(AActor* Target) const;


	/************************************************************************/
	/* FindNearestActor                                                     */
	/************************************************************************/
	AActor* FindNearestActor(TArray<AActor*> TestActors) const;

#pragma endregion

};
