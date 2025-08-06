// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/WidgetComponent.h"
#include "Gameplay/UI/Player/HealthBarWidget.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogEnemyBase, Log, All);

AEnemyBase::AEnemyBase()
{
	healthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	healthWidget->SetupAttachment(GetMesh(), TEXT("head"));
	
	healthWidget->SetRelativeLocation(FVector(25.f, 0.f, 0.f));
	healthWidget->SetRelativeRotation(FRotator(270.f, 90.f, 0.f));
	
	healthWidget->SetWidgetSpace(EWidgetSpace::Screen);
	healthWidget->SetDrawSize(FVector2D(50.f, 4.f));
	healthWidget->SetDrawAtDesiredSize(false);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (UUserWidget* widgetInstance = healthWidget->GetUserWidgetObject())
	{
		cachedHealthBarWidget = Cast<UHealthBarWidget>(widgetInstance);
	}
	else UE_LOG(GPLogEnemyBase, Error, TEXT("[%s] [OnHealthChanged] GetUserWidgetObject returned nullptr"), *GetName());
	
	SetHealthBarWidgetVisibility(false);
}

void AEnemyBase::SetHealthBarWidgetVisibility(bool visible)
{
	if (healthWidget->IsVisible() == visible) return;
	
	healthWidget->SetVisibility(visible);
}

void AEnemyBase::OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta)
{
	if (cachedHealthBarWidget)
	{
		cachedHealthBarWidget->UpdateHealth(newHealth);
	}
	else UE_LOG(GPLogEnemyBase, Error, TEXT("[%s] [OnHealthChanged] cachedHealthBarWidget returned nullptr"), *GetName());
}

FPerformDeath AEnemyBase::PerformDeath()
{
	SetHealthBarWidgetVisibility(false);
	return Super::PerformDeath();
}
