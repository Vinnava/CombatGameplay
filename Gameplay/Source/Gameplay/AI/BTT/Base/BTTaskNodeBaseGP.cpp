// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNodeBaseGP.h"

#include "BlueprintNodeHelpers.h"


UBTTaskNodeBaseGP::UBTTaskNodeBaseGP()
{
	const UClass* StopAtClass = UBTTaskNodeBaseGP::StaticClass();
	bShowPropertyDetails = true;

	// all blueprint based nodes must create instances
	bCreateNodeInstance = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		BlueprintNodeHelpers::CollectPropertyData(this, StopAtClass, PropertyData);
	}
}

FString UBTTaskNodeBaseGP::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	UBTTaskNodeBaseGP* CDO = static_cast<UBTTaskNodeBaseGP*>(GetClass()->GetDefaultObject());
	if (bShowPropertyDetails && CDO)
	{
		const UClass* StopAtClass = UBTTaskNodeBaseGP::StaticClass();
		FString PropertyDesc = BlueprintNodeHelpers::CollectPropertyDescription(this, StopAtClass, CDO->PropertyData);
		if (PropertyDesc.Len())
		{
			ReturnDesc += TEXT(":\n\n");
			ReturnDesc += PropertyDesc;
		}
	}
	return ReturnDesc;
}


