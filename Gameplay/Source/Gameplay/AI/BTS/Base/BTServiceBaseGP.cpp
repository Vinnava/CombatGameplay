// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceBaseGP.h"

#include "BlueprintNodeHelpers.h"
#include "BehaviorTree/BTFunctionLibrary.h"


UBTServiceBaseGP::UBTServiceBaseGP()
{
	
	UClass* StopAtClass = UBTServiceBaseGP::StaticClass();
	bShowPropertyDetails = true;

	// all blueprint based nodes must create instances
	bCreateNodeInstance = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		BlueprintNodeHelpers::CollectPropertyData(this, StopAtClass, PropertyData);
	}
}

FString UBTServiceBaseGP::GetStaticServiceDescription() const
{
	FString ReturnDesc;

	UBTServiceBaseGP* CDO = (UBTServiceBaseGP*)(GetClass()->GetDefaultObject());
	if (CDO && bShowPropertyDetails)
	{
		UClass* StopAtClass = UBTServiceBaseGP::StaticClass();
		FString PropertyDesc = BlueprintNodeHelpers::CollectPropertyDescription(this, StopAtClass, CDO->PropertyData);
		if (PropertyDesc.Len())
		{
			ReturnDesc += TEXT("\n");
			ReturnDesc += PropertyDesc;
		}
	}
	return ReturnDesc;
}

