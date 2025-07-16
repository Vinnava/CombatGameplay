// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagLibrary.generated.h"


namespace GameplayTags
{
	namespace Action
	{
		inline const FGameplayTag& LightAttack()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Attack.LightAttack"));
			return Tag;
		}
		inline const FGameplayTag& Dodge()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Dodge"));
			return Tag;
		}
		inline const FGameplayTag& Die()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Die"));
			return Tag;
		}
	}
    
	namespace State
	{
		inline const FGameplayTag& Dead()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dead"));
			return Tag;
		}
		inline const FGameplayTag& Attacking()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Attacking"));
			return Tag;
		}
		inline const FGameplayTag& Disabled()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Disabled"));
			return Tag;
		}
		inline const FGameplayTag& Dodging()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dodging"));
			return Tag;
		}
		inline const FGameplayTag& GeneralAction()
		{
			static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.GeneralAction"));
			return Tag;
		}
	}
    
	inline const FGameplayTag& Player()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Player"));
		return Tag;
	}
}
UCLASS()
class GAMEPLAY_API UGameplayTagLibrary : public UObject
{
	GENERATED_BODY()

	static const FGameplayTag& GetLightAttackActionTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Attack.LightAttack"));
		return Tag;
	}
    
	static const FGameplayTag& GetDodgeActionTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Dodge"));
		return Tag;
	}
    
	static const FGameplayTag& GetDieActionTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Die"));
		return Tag;
	}
    
	// State Tags
	static const FGameplayTag& GetDeadStateTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dead"));
		return Tag;
	}
    
	static const FGameplayTag& GetAttackingStateTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Attacking"));
		return Tag;
	}
    
	static const FGameplayTag& GetDisabledStateTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Disabled"));
		return Tag;
	}
    
	static const FGameplayTag& GetDodgingStateTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dodging"));
		return Tag;
	}
    
	static const FGameplayTag& GetGeneralActionStateTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.GeneralAction"));
		return Tag;
	}
    
	static const FGameplayTag& GetPlayerTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Character.Player"));
		return Tag;
	}
};
