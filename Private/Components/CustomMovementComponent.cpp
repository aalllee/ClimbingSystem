// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"
#include "ClimbingSystem/DebugHelper.h"
#pragma region ClimbTraces
void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType, ThisTickFunction);
	//TraceClimbableSurfaces();
	//TraceFromEyeHeight(100.f);

}
TArray<FHitResult> UCustomMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start,const FVector& End, bool bShowDebug, bool bDrawPersistentShapes)
{

	TArray<FHitResult> OutCapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if (bShowDebug)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
		);

	return OutCapsuleTraceHitResults;
	
}
FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebug, bool bDrawPersistentShapes)
{
	FHitResult OutHit;
	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if (bShowDebug)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
	);
	return OutHit;
}
#pragma endregion


#pragma region ClimbCore
bool UCustomMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start, End, true,true);
	return !ClimbableSurfacesTracedResults.IsEmpty();
}
FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start, End, true,true);

}
bool UCustomMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceClimbableSurfaces()) return false;
	if (!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
}
void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{

		Debug::Print(TEXT("Enable Climbing"));
		if (CanStartClimbing())
		{
			Debug::Print(TEXT("Can Start Climbing"));
		}
		else
		{
			Debug::Print(TEXT("CanNOT Start Climbing"));
		
		}
	
	}
	else 
	{
		
	}
}
bool UCustomMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}
#pragma endregion