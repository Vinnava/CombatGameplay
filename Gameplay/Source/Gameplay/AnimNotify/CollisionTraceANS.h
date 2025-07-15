// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CollisionTraceANS.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UCollisionTraceANS : public UAnimNotifyState
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual FLinearColor GetEditorColor() override;
#endif

	virtual void NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration,
							const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation,
						const FAnimNotifyEventReference& eventReference) override;
};
