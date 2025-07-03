// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/Camera/CameraShake/AttackCameraShake.h"

UAttackCameraShake::UAttackCameraShake()
{
	// Oscillation Settings
	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 3.0f;

	// Example Rotation Oscillation
	RotOscillation.Pitch.Amplitude = 10.0f;
	RotOscillation.Pitch.Frequency = 25.0f;

	RotOscillation.Yaw.Amplitude = 5.0f;
	RotOscillation.Yaw.Frequency = 20.0f;
	
}

