// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Gameplay/Components/CollisionComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	collisionComp = CreateDefaultSubobject<UCollisionComponent>("CollisionComp");
}
