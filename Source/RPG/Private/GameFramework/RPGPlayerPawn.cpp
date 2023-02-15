// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RPGPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
ARPGPlayerPawn::ARPGPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetSphereRadius(66.f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetRootComponent(SphereCollider);

	// Create a camera boom...
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	SpringArm->TargetArmLength = 800.f;
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = true;

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	

	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void ARPGPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	CameraTargetZoom = SpringArm->TargetArmLength;
	DefaultZoom = CameraTargetZoom;
	CameraTargetRotation = SphereCollider->GetRelativeRotation().Yaw;
	DefaultRotation = CameraTargetRotation;
	DefaultCamPitch = SpringArm->GetRelativeRotation().Pitch;
	CameraDepitchHeight = CameraMaxHeight - ((CameraMaxHeight - CameraMinHeight) * .33);
}

void ARPGPlayerPawn::MoveForward(float Delta)
{
	if (Delta == 0)
	{
		return;
	}
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	SpringArmRotation.Pitch = 0.f;
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(SpringArmRotation);
	ForwardVector *= Delta * MoveSpeed * 1.25f;
	ForwardVector.Z = 0;
	FVector Location = GetActorLocation();
	Location += ForwardVector;
	SetActorLocation(Location);
}

void ARPGPlayerPawn::MoveLateral(float Delta)
{
	if (Delta == 0)
	{
		return;
	}
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	SpringArmRotation.Pitch = 0.f;

	FVector RightVector = UKismetMathLibrary::GetRightVector(SpringArmRotation);
	RightVector *= Delta * MoveSpeed;
	RightVector.Z = 0;
	FVector Location = GetActorLocation();
	Location += RightVector;
	SetActorLocation(Location);
}

void ARPGPlayerPawn::ZoomCamera(float Delta)
{
	if (Delta == 0)
	{
		return;
	}
	float CurrentHeight = SpringArm->TargetArmLength;
	CameraTargetZoom = FMath::Clamp(CurrentHeight + (Delta * CameraZoomSpeed), CameraMinHeight, CameraMaxHeight);
}

void ARPGPlayerPawn::RotateCamera(float Delta)
{
	if (Delta == 0)
	{
		return;
	}
	float CurrentRotation = SpringArm->GetRelativeRotation().Yaw;
	CameraTargetRotation = CurrentRotation += Delta * CameraRotateSpeed;
}

void ARPGPlayerPawn::ResetCamera()
{
	CameraTargetZoom = DefaultZoom;
	CameraTargetRotation = DefaultRotation;
}

// Called every frame
void ARPGPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		if (CameraTargetZoom != SpringArm->TargetArmLength)
		{
			SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, CameraTargetZoom, DeltaTime, ZoomInterpSpeed);
			FRotator NewRotation = SpringArm->GetRelativeRotation();
			//if the Camera is beyond the depitch height, set the pitch back to its default value
			if (SpringArm->TargetArmLength > CameraDepitchHeight)
			{
				NewRotation.Pitch = DefaultCamPitch;
			}
			else
			{
				//if the camera is under the depitch height, gradually change its pitch so that it reaches the min pitch at the min height
				//e.g. if MinHeight = 200, DepitchHeight = 400, MinPitch = 20 and DefaultPitch = 60, we want to be equal to 60 at 400 and 20 at 200
				//60 - ((Diff) * (400 - CurrentLength) / MinHeight
				float CurrentLength = SpringArm->TargetArmLength;
				NewRotation.Pitch = DefaultCamPitch + ((DefaultCamPitch*-1) - CameraMinPitch) * (CameraDepitchHeight - CurrentLength) / (CameraDepitchHeight - CameraMinHeight);
				//UE_LOG(LogTemp, Log, TEXT("%f"), NewRotation.Pitch);

			}

			SpringArm->SetRelativeRotation(NewRotation);
		}

		if (CameraTargetRotation != SphereCollider->GetRelativeRotation().Yaw)
		{
			FRotator CurrentRotation = SpringArm->GetRelativeRotation();
			float NewRotation = FMath::FInterpTo(CurrentRotation.Yaw, CameraTargetRotation, DeltaTime, RotationInterpSpeed);
			CurrentRotation.Yaw = NewRotation;
			SpringArm->SetRelativeRotation(CurrentRotation);
		}
	}
	

}

// Called to bind functionality to input
void ARPGPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetCamera", IE_Released, this, &ARPGPlayerPawn::ResetCamera);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveLateral", this, &ARPGPlayerPawn::MoveLateral);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &ARPGPlayerPawn::ZoomCamera);
	PlayerInputComponent->BindAxis("RotateCamera", this, &ARPGPlayerPawn::RotateCamera);
}



