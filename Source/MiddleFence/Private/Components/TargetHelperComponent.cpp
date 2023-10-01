// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetHelperComponent.h"
#include "DrawDebugHelpers.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "Game/MiddleFenceGameplayStatics.h"

// Sets default values for this component's properties
UTargetHelperComponent::UTargetHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;

	DefaultSearchClass = AActor::StaticClass();
	DefaultSearchRadius = 1500.0f;
	DefaultSearchChannel = ECC_Pawn;
	DefaultTargetSelectionMethod = ETargetSelectionType::TST_Both;

	DefaultAngleAcceptRange = FVector2D(0, 40);

	LostTarget = false;
	LostDistance = 2000.0f;
}


// Called when the game starts
void UTargetHelperComponent::BeginPlay()
{
	Super::BeginPlay();
}

TArray<AActor*> UTargetHelperComponent::FindActorsWithinRadius() const
{
	AActor* Owner = GetOwner();

	if (Owner)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector SearchLocation = Owner->GetActorLocation();
			TArray<FOverlapResult> Out;

			FCollisionObjectQueryParams COQP;
			COQP.AddObjectTypesToQuery(DefaultSearchChannel);

			FCollisionQueryParams CQP;
			CQP.AddIgnoredActor(Owner);

			GetWorld()->OverlapMultiByObjectType(Out, SearchLocation, FQuat::Identity, COQP, FCollisionShape::MakeSphere(DefaultSearchRadius), CQP);

			if(bDrawTrace)
				DrawDebugSphere(World, SearchLocation, DefaultSearchRadius, 8, TraceDrawColor, false, DrawliveTime);

			TArray<AActor*> Result = TArray<AActor*>();
			if (Out.Num() > 0)
			{
				for (FOverlapResult item : Out)
				{
					if (DefaultSearchClass && item.GetActor()->IsA(DefaultSearchClass) && !Result.Contains(item.GetActor()))
					{
						if (bCheckTargetHealth)
						{
							AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(item.GetActor());
							if (advanceCharacter && advanceCharacter->IsDead())
								continue;
						}

						Result.Add(item.GetActor());

						if (bDrawTrace)
							DrawDebugSphere(World, item.GetActor()->GetActorLocation(), 50, 4, FColor::Blue, false, DrawliveTime / 2.0f);
					}
				}
			}
			return Result;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s : %s - World Is Null"), *GetNameSafe(Owner), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s : %s - Owner Is Null"), *GetNameSafe(Owner), *GetNameSafe(this));
	}
	
	return TArray<AActor*>();
}

AActor* UTargetHelperComponent::SelectTarget(TArray<AActor*> AllActors) const
{
	if (AllActors.Num() == 0)
		return nullptr;

	TArray<AActor*> AngleAcceptableActors = TArray<AActor *>();
	AActor* ClosestActor = nullptr;
	float ClosestAngle = 360;


	//test by angle
	for (AActor* item : AllActors)
	{

		float Angle = 360;

		if (bSearchByPlayerCamera)
		{
			bool bSearchByPlayerCameraIsDone = false;

			if (GetOwner())
			{
				ACharacter* ch = Cast< ACharacter>(GetOwner());
				if (ch)
				{
					APlayerController* pc = Cast< APlayerController>(ch->GetController());
					if (pc)
					{
						bSearchByPlayerCameraIsDone = true;
						Angle = UMiddleFenceGameplayStatics::GetAbsoluteAngleBetweenCameraPointAndTarget2D(item, pc);
						//UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), Angle);
					}
				}
			}
			
			if(bSearchByPlayerCameraIsDone == false)
			{
				Angle = GetAbsoluteAngle(item);
			}
		}
		else
		{
			Angle = GetAbsoluteAngle(item);
		}

		//UE_LOG(LogTemp, Warning, TEXT("%s -> Angle %f"), *GetNameSafe(item), Angle);

		//if (Angle >= DefaultAngleAcceptRange.X && Angle <= DefaultAngleAcceptRange.Y)
		//{
			AngleAcceptableActors.Add(item);

			if (Angle < ClosestAngle)
			{
				ClosestActor = item;
				ClosestAngle = Angle;
			}
		//}
	}

	/*If there is no actor within angle then test only by distance*/
	/*if (AngleAcceptableActors.Num() == 0)
	{
		AngleAcceptableActors = AllActors;
	}*/

	//test by distance
	//AActor* NearestActor = FindNearestActor(AngleAcceptableActors);
	//UE_LOG(LogTemp, Warning, TEXT("Selected Actor : %s"), *GetNameSafe(NearestActor));

	return ClosestActor;
}

bool UTargetHelperComponent::FindAndAssignTarget()
{
	AActor* DesireTarget = SelectTarget(FindActorsWithinRadius());
	UWorld* World = GetWorld();

	if (DesireTarget != nullptr)
	{
		FVector GoodPositionForDebugText = (DesireTarget->GetActorLocation() + FVector(0, 0, 120));

		if (World)
		{
			if (bDrawTrace)
			{
				DrawDebugString(World, GoodPositionForDebugText, "Current Target", nullptr, FColor::Green, DrawliveTime, false, 1.25f);
				DrawDebugSphere(World, DesireTarget->GetActorLocation(), 55, 8, FColor::Green, false, DrawliveTime);
			}

			if (LostTarget)
			{
				//UE_LOG(LogTemp, Warning, TEXT("TH_LostTargetTimer is valid %d"), TH_LostTargetTimer.IsValid());

				if (TH_LostTargetTimer.IsValid())
				{
					World->GetTimerManager().ClearTimer(TH_LostTargetTimer);
				}

				World->GetTimerManager().SetTimer(TH_LostTargetTimer, this, &UTargetHelperComponent::KeepTrackOfTarget, World->GetDeltaSeconds(), true);

				//UE_LOG(LogTemp, Warning, TEXT("Delta Second : %f"), World->GetDeltaSeconds());
			}
		}

		CurrentTarget = DesireTarget;
		return true;
	}

	return false;
}

AActor* UTargetHelperComponent::GetCurrentTarget() const
{
	return CurrentTarget;
}

void UTargetHelperComponent::ClearCurrentTarget()
{
	CurrentTarget = nullptr;
}

void UTargetHelperComponent::KeepTrackOfTarget()
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();

	if (World && Owner && CurrentTarget)
	{
		float DistanceToTarget = FVector::Distance(Owner->GetActorLocation(), CurrentTarget->GetActorLocation());

		if (DistanceToTarget > LostDistance)
		{
			if (TH_LostTargetTimer.IsValid())
			{
				World->GetTimerManager().ClearTimer(TH_LostTargetTimer);
			}

			OnLostTarget();
		}

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, "KeepTrackOfTarget : " + FString::SanitizeFloat(DistanceToTarget));
	}
}

void UTargetHelperComponent::OnLostTarget()
{
	UWorld* World = GetWorld();

	if (World && TH_LostTargetTimer.IsValid())
	{
		World->GetTimerManager().ClearTimer(TH_LostTargetTimer);
	}

	CurrentTarget = nullptr;
}

float UTargetHelperComponent::GetAbsoluteAngle(AActor* Target) const
{
	AActor* Owner = GetOwner();

	if (Target && Owner)
	{
		FVector Dir = Target->GetActorLocation() - GetOwner()->GetActorLocation();
		Dir.Normalize();

		float DirDot = FVector::DotProduct(Dir, Owner->GetActorForwardVector());
		float AbsoluteAngle = FMath::Acos(DirDot) * (180.0f / PI);

		return AbsoluteAngle;
	}

	return -1;
}

AActor* UTargetHelperComponent::FindNearestActor(TArray<AActor*> TestActors) const
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		float NearestDistance = DefaultSearchRadius + 100;
		AActor* NearestActor = nullptr;

		for (AActor* item : TestActors)
		{
			float Dis = FVector::Distance(item->GetActorLocation(), Owner->GetActorLocation());
			if (Dis < NearestDistance)
			{
				NearestDistance = Dis;
				NearestActor = item;
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("NearestActor : %s"), *GetNameSafe(NearestActor));
		return NearestActor;
	}

	return nullptr;
}

