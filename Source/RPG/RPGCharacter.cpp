// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCharacter.h"
#include "RPGPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "GameFramework/RPGGameState.h"
#include "GameState/Encounter.h"
#include "GameState/Turn.h"
#include "Net/UnrealNetwork.h"
#include "UI/SelectionDecalComponent.h"
#include "Interfaces/PlayerInteraction/PlayerControlComponent.h"
#include "GameplayActionSystem/GameplayActionComponent.h"
#include "InventorySystem/InventoryComponent.h"
#include "Perception/PawnSensingComponent.h"

ARPGCharacter::ARPGCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	//Setup the Decal for Selection
	SelectionDecalComponent = CreateDefaultSubobject<USelectionDecalComponent>(TEXT("SelectionDecalComponent"));
	SelectionDecalComponent->SetupAttachment(RootComponent);
	SelectionDecalComponent->SetVisibility(false);
	SelectionDecalComponent->DecalSize = FVector(20, GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.25f, GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.25f);
	SelectionDecalComponent->SetRelativeRotation(FRotator(90, 0, 0));
	SelectionDecalComponent->SetRelativeLocation(FVector(0, 0, -100.f));


	PlayerControlComp = CreateDefaultSubobject<UPlayerControlComponent>(TEXT("PlayerControlComponent"));
	PlayerControlComp->SetIsPlayerControlled(true);
	PlayerControlComp->SetIsReplicated(true);

	ActionComponent = CreateDefaultSubobject<UGameplayActionComponent>("ActionComponent");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComponent"));
	SensingComponent->SetPeripheralVisionAngle(70.f);

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ARPGCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

bool ARPGCharacter::IsActiveTurnCharacter()
{
	if (!GetCharacterTurn() || !GetCharacterTurn()->GetEncounter().IsValid())
	{
		return false;
	}
	AEncounter* Encounter = GetCharacterTurn()->GetEncounter().Get();

	return Encounter->GetCurrentTurn() == GetCharacterTurn();
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	if (GetCharacterTurn())
	{
		OnEncounterJoined.Broadcast(this, GetCharacterTurn()->GetEncounter().Get());
	}
}

bool ARPGCharacter::IsInEncounter_Implementation()
{
	if (CharacterTurn)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ARPGCharacter::GetOnHover_Implementation()
{
	return bOnSelectionHover;
}

void ARPGCharacter::SetOnHover_Implementation(bool OnHover)
{
	if (bIsSelected)
	{
		return;
	}
	SetDecalMaterial();
	bOnSelectionHover = OnHover;
	if (bOnSelectionHover)
	{
		SelectionDecalComponent->SetVisibility(true);
	}
	else
	{
		SelectionDecalComponent->SetVisibility(false);
	}
}

void ARPGCharacter::Deselect_Implementation(ARPGPlayerController* Selector)
{
	SelectionDecalComponent->SetVisibility(false);
	bIsSelected = false;
}

void ARPGCharacter::Select_Implementation(ARPGPlayerController* Selector)
{
	bIsSelected = true;
	SetDecalMaterial();

	SelectionDecalComponent->SetVisibility(true);

}

bool ARPGCharacter::CanSelect_Implementation(ARPGPlayerController* Selector)
{
	return true;
}


UAnimInstance* ARPGCharacter::GetOwnerAnimInstance_Implementation()
{
	return GetMesh()->GetAnimInstance();
}

APlayerController* ARPGCharacter::GetControllingPlayer_Implementation()
{
	if (PlayerControlComp->IsPlayerControlled())
	{
		return Cast<APlayerController>(PlayerControlComp->GetPlayerController());
	}
	return nullptr;
}

void ARPGCharacter::SetDecalMaterial()
{
	if (PlayerControlComp->IsPlayerControlled())
	{

		//char is controlled by the local player controller
		SelectionDecalComponent->SetDecalMaterial(SelectionDecalComponent->GetControlledDecalMaterial());
	}
	else
	{
		//MODIFY WHEN FACTIONS ARE ADDED - IF AI IS HOSTILE FACTION, RED, IF NEUTRAL, NEUTRAL
		if (true)
		{
			SelectionDecalComponent->SetDecalMaterial(SelectionDecalComponent->GetEnemyDecalMaterial());
		}
		else
		{
			SelectionDecalComponent->SetDecalMaterial(SelectionDecalComponent->GetNeutralDecalMaterial());
		}
	}
}

void ARPGCharacter::EndCharacterTurn()
{
	Server_EndCharacterTurn();
}

void ARPGCharacter::Server_EndCharacterTurn_Implementation()
{
	//check that the data needed to end turn is valid
	if (!GetCharacterTurn() || !GetCharacterTurn()->GetEncounter().IsValid())
	{
		return;
	}
	
	//check that I am current turn character
	/*if (!IsActiveTurnCharacter())
	{
		return;
	}*/

	AEncounter* Encounter = GetCharacterTurn()->GetEncounter().Get();
	Encounter->EndCurrentTurn();
}

UTurn* ARPGCharacter::GetCharacterTurn() const
{
	return ActionComponent->GetTurn();
}

void ARPGCharacter::SetCharacterTurn(UTurn* val)
{
	ActionComponent->SetTurn(val);
}

void ARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGCharacter, CharacterTurn);
}
