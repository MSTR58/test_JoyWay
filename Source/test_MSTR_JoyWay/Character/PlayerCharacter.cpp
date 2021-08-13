#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"											// Capsule Component
#include "Components/InputComponent.h"												// PlayerInput Component
#include "Camera/CameraComponent.h"													// Camera Component
#include "Net/UnrealNetwork.h"														// DOREPLIFETIME
#include "../Library/MSTRLibrary.h"													// MSTRLibrary
#include "DrawDebugHelpers.h"														// Drow Trace
#include "../Library/InteractiveActor.h"											// Interactive Actor
#include "../Character/EquipmentComponent.h"										// Equipment
#include "Kismet/GameplayStatics.h"
#include "../Gameplay/GameModeArena.h"
#include "GameFramework/SpringArmComponent.h"


APlayerCharacter::APlayerCharacter()
{	
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	NetCullDistanceSquared = 60000000.0f;
	bUseControllerRotationYaw = true;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);

	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector{0.0f,0.0f,70.f});
	SpringArm->TargetArmLength = 0.0f;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;
	Camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	Camera->PostProcessSettings.MotionBlurAmount = 0.f;

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
	FPSMesh->SetupAttachment(Camera);
	FPSMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FPSMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
}


void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(!HasAuthority())
	{
		GetTPSMesh()->SetOwnerNoSee(true);
		GetFPSMesh()->SetOnlyOwnerSee(true);
	}
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		ReplicatedControllerPitch = GetControlRotation().Pitch;
	}
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("MainWeapon", IE_Pressed, this, &APlayerCharacter::SelectMainWeapon);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &APlayerCharacter::SelectSecondaryWeapon);
	PlayerInputComponent->BindAction("Knife", IE_Pressed, this, &APlayerCharacter::SelectKnife);
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &APlayerCharacter::DropWeapon);
	PlayerInputComponent->BindAction("WeaponFire", IE_Pressed, this, &APlayerCharacter::WeaponFire);
	PlayerInputComponent->BindAction("WeaponFire", IE_Released, this, &APlayerCharacter::StopWeaponFire);
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, ReplicatedControllerPitch);
	DOREPLIFETIME(APlayerCharacter, Health);
}


void APlayerCharacter::OnRep_Health()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		const FString sms = FString::SanitizeFloat(Health);
		UKismetSystemLibrary::PrintString(this, "Health = "+sms, true, true, FLinearColor::Green, 2.0f);

		if (Health <= 0.0f)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
			if (PlayerController)
			{
				DisableInput(PlayerController);
			}
		}
	}
}


float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.0f)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
		AGameModeArena* GameMode = Cast<AGameModeArena>(UGameplayStatics::GetGameMode(this));
		if (PlayerController && GameMode)
		{
			PlayerController->DisableInput(PlayerController);
			GetEquipmentComponent()->OnPlayerDie();
			GameMode->RespawnCharacter(PlayerController);
		}
	}
	return DamageAmount;
}


FVector APlayerCharacter::GetCameraLocation() const
{
	return Camera->GetComponentLocation();
}


// Player Input


void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}


void APlayerCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void APlayerCharacter::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerPitchInput(Value);
	}
}


void APlayerCharacter::LookRight(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value);
	}
}


void APlayerCharacter::SelectMainWeapon()
{
	GetEquipmentComponent()->ChangeWeapon(EWeaponType::Main);
}


void APlayerCharacter::SelectSecondaryWeapon()
{
	GetEquipmentComponent()->ChangeWeapon(EWeaponType::Secondary);
}


void APlayerCharacter::SelectKnife()
{
	GetEquipmentComponent()->ChangeWeapon(EWeaponType::Knife);
}


void APlayerCharacter::DropWeapon()
{
	AWeaponBase* Weapon = GetEquipmentComponent()->GetHandWeapon();
	if (Weapon)
	{
		GetEquipmentComponent()->DropWeaponServer();
	}
}


void APlayerCharacter::WeaponFire()
{
	GetEquipmentComponent()->WeaponFire(true);
}


void APlayerCharacter::StopWeaponFire()
{
	GetEquipmentComponent()->WeaponFire(false);
}


// Interact Mechanics


AActor* APlayerCharacter::LineTraceForward()
{
	FVector Start = GetCameraLocation() + GetOwner()->GetActorForwardVector() * 100.f;
	FVector End = Start + GetOwner()->GetActorForwardVector() * 250.f;
	
	FHitResult Hit;
	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);
	
	if (bHit)
	{
		DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(3,3,3), FColor::Black, false, 5.0f);
		return Hit.GetActor();
	}
	else return nullptr;
}


void APlayerCharacter::Interact()
{
	AActor* Result = LineTraceForward();

	if (Result)
	{
		IInteractiveActor* TargetActor = Cast<IInteractiveActor>(Result);
		if (TargetActor && TargetActor->CanInteract(this))
		{
			if (HasAuthority())
			{
				TargetActor->Interact(this);
			}
			else
			{
				ServerInteract();
			}
		}
	}
}


void APlayerCharacter::ServerInteract_Implementation()
{
	Interact();
}



