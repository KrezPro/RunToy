// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToyRunnerCharacter.h"

#include "AIMobController.h"
#include "EngineUtils.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyToy.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AToyRunnerCharacter::AToyRunnerCharacter()
{
	// Инициализация капсульного компонента (коллизия)
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Создание и настройка камеры от первого лица
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); 
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	// Инициализация переменных для игрушки
	CurrentToyType = EToyType::Cube;
	bToyActive = false;
	CurrentToyActor = nullptr;

	// Разрешаем репликацию
	bReplicates = true;
}


void AToyRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//////////////////////////////Действия с игрушкой//////////////////////////////////////////////
 bool AToyRunnerCharacter::IsHoldingToy() const
 {
 	return (CurrentToyActor != nullptr);
 }

void AToyRunnerCharacter::PickupToy_Implementation(AMyToy* Toy)
{
	if (CurrentToyActor != nullptr)
	{
		return;
	}
	if(!Toy)
	{
		return;
	}
	GetCapsuleComponent()->SetGenerateOverlapEvents(false); //Отключаем возможность HitEvent Toy
	
	// Название сокета, к которому будем прикреплять игрушку (без этого не заработает)
	FName SocketName = "ToySocket"; 
	
	if (!GetMesh()->DoesSocketExist(SocketName))
	{
		return;
	}
	
	// Отключаем физику и коллизию перед прикреплением
	UPrimitiveComponent* ToyPrimitive = Cast<UPrimitiveComponent>(Toy->GetRootComponent());
	if (ToyPrimitive)
	{
		ToyPrimitive->SetSimulatePhysics(false);
		ToyPrimitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	Toy->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	//Размер игрушки в руке 
	FVector NewScale = Toy->GetActorScale3D() - FVector(0.25f, 0.25f, 0.25f);
	Toy->SetActorScale3D(NewScale);

	
	// Устанавливаем игрушку как активную
	bToyActive = true;
	CurrentToyActor = Toy;
}

void AToyRunnerCharacter::ThrowToy_Implementation()
{
	{
		if (!bToyAvailableForThrow) // Если игрушку уже бросили, другие не могут бросать
		{
			return;
		}
		
		FTimerHandle FireRateTimerHandle; //Таймер проигрывания анимации
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(FireRateTimerHandle))
		{
			return;
		}
		if (!CurrentToyActor)
		{
			return;
		}
		
		FName SocketName = "ToySocket";
		if (!GetMesh()->DoesSocketExist(SocketName))
		{
			return;
		}
		if(HasAuthority())
		{
			bIsAiming = true;
		}
		bIsAiming = true;
		
		//Запрещаем игрокам бросать
		for (TActorIterator<AToyRunnerCharacter> It(GetWorld()); It; ++It)
		{
			It->bToyAvailableForThrow = false;
		}
		
		if (!TimerManager.IsTimerActive(FireRateTimerHandle)){

			GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, [this]()
			{
				
			TSubclassOf<AMyToy> ToyClassToSpawn = nullptr;

			// Проверяем, есть ли класс текущей игрушки в массиве ToyClassesToSpawn
			for (auto& ToyClass : ToyClassesToSpawn)
			{
				if(ToyClass)
				{
					if(CurrentToyActor)
					{
						if (ToyClass && ToyClass->GetName() == CurrentToyActor->GetClass()->GetName())
						{
							ToyClassToSpawn = ToyClass;
							break;
						}
					}
				}
			}

			// Если нашли класс, спауним игрушку
			if (ToyClassToSpawn)
			{
				FVector SpawnLocation = CurrentToyActor->GetActorLocation(); 
					FRotator SpawnRotation = CurrentToyActor->GetActorRotation();
		
				// Спауним новую игрушку
				auto SpawnedToy = GetWorld()->SpawnActor<AMyToy>(
					ToyClassToSpawn, SpawnLocation, SpawnRotation);
				UPrimitiveComponent* ToyPrimitive = Cast<UPrimitiveComponent>(SpawnedToy->GetRootComponent());
				FVector ThrowDirection = GetActorForwardVector();
				ToyPrimitive->AddImpulse(ThrowDirection * 1500.f, NAME_None, true);
				
		    	}
				if(HasAuthority())
				{
					bIsAiming = false;
				}
				bIsAiming = false;
					
				if(CurrentToyActor)
				{
					CurrentToyActor->Destroy();
				}
					
					CurrentToyActor = nullptr;
				
				//Таймер для полета игрушки без коллизий (отключены оверлапы для игрока)
				     FTimerHandle FireRateTimerHandle3; 
					 FTimerManager& TimerManager = GetWorld()->GetTimerManager();
					 if (!TimerManager.IsTimerActive(FireRateTimerHandle3))
					 {
						 GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle3, [this]()
						 {
							 GetCapsuleComponent()->SetGenerateOverlapEvents(true); //Возвращаем возможность HitEvent и Overlap
					 	
						 }, 1.64, false);
					 }
			}, 0.64, false);
		}
	}
}

//////////////////////////////Управление игроком//////////////////////////////////////////////

void AToyRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Привязка осей движения
	PlayerInputComponent->BindAxis("MoveForward", this, &AToyRunnerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AToyRunnerCharacter::MoveRight);

	// Привязка осей мыши для управления камерой
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Привязка действий для прыжка
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AToyRunnerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AToyRunnerCharacter::StopJump);

	// Привязка действия для броска игрушки
	PlayerInputComponent->BindAction("ThrowToy", IE_Pressed, this, &AToyRunnerCharacter::ThrowToy);
}

void AToyRunnerCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AToyRunnerCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AToyRunnerCharacter::StartJump()
{
	bPressedJump = true;
}

void AToyRunnerCharacter::StopJump()
{
	bPressedJump = false;
}

void AToyRunnerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToyRunnerCharacter, CurrentToyType);
	DOREPLIFETIME(AToyRunnerCharacter, bToyActive);
	DOREPLIFETIME(AToyRunnerCharacter, CurrentToyActor);
	DOREPLIFETIME(AToyRunnerCharacter, bIsAiming);
	DOREPLIFETIME(AToyRunnerCharacter, bToyAvailableForThrow);
}
