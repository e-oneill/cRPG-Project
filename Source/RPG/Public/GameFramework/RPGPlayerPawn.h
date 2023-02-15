// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RPGPlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USphereComponent;

UCLASS()
class RPG_API ARPGPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARPGPlayerPawn();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollider;

	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArm;
	#pragma region CameraControls
	void MoveForward(float Delta);
	void MoveLateral(float Delta);
	void ZoomCamera(float Delta);
	void RotateCamera(float Delta);
	void ResetCamera();
	#pragma region CameraControlSetupVariables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraMinHeight = 250.f;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraDepitchHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraMinPitch = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraMaxHeight = 1200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraZoomSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ZoomInterpSpeed = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraRotateSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotationInterpSpeed = 50.f;
	#pragma endregion CameraControlSetupVariables
	
	float CameraTargetZoom;
	float CameraTargetRotation;
	float DefaultZoom;
	float DefaultRotation;
	float DefaultCamPitch;

	#pragma endregion CameraControls

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
