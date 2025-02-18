#include "AIMob.h"
#include "AIController.h"
#include "AIMobController.h"
#include "MyGameStateBase.h"
#include "Animation/BlendSpace.h"
#include "NavigationSystem.h"
#include "Components/TextRenderComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AAIMob::AAIMob()
{
	PrimaryActorTick.bCanEverTick = false;

	// ✅ Создаём `NameTag`, но проверяем его существование
	NameTag = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameTag"));
	if (NameTag)
	{
		NameTag->SetupAttachment(RootComponent);
		NameTag->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
		NameTag->SetHorizontalAlignment(EHTA_Center);
		NameTag->SetWorldSize(40.f);
		NameTag->SetText(FText::FromString("Zombie"));
		NameTag->SetTextRenderColor(FColor::Red);
	}
}

void AAIMob::BeginPlay()
{
	Super::BeginPlay();

	// ✅ Проверяем `NameTag` перед использованием
	if (!NameTag)
	{
		NameTag = NewObject<UTextRenderComponent>(this);
		if (NameTag)
		{
			NameTag->RegisterComponent();
			NameTag->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			NameTag->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
			NameTag->SetHorizontalAlignment(EHTA_Center);
			NameTag->SetWorldSize(40.f);
			NameTag->SetText(FText::FromString("Zombie"));
			NameTag->SetTextRenderColor(FColor::Red);
		}
		else
		{
			return;
		}
	}

	// ✅ Сервер присваивает имя
	if (HasAuthority()) 
	{
		AssignRandomName();
	}

	// ✅ Проверяем, не пустое ли имя
	if (MobName.IsEmpty())
	{
		MobName = "UnknownMob";
	}

	// ✅ Устанавливаем имя
	NameTag->SetText(FText::FromString(MobName));

	// ✅ Проверяем `CharacterMovementComponent`
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	// ✅ Устанавливаем случайную скорость
	MoveComp->MaxWalkSpeed = FMath::RandRange(100.f, 600.f);
}

void AAIMob::OnRep_MobName()
{

	// ✅ Проверяем `NameTag`
	if (!NameTag)
	{
		return;
	}

	// ✅ Устанавливаем имя
	NameTag->SetText(FText::FromString(MobName));
}

void AAIMob::AssignRandomName()
{
	static const TArray<FString> PossibleNames = {
		TEXT("Fleshgnawer"), TEXT("Rotfang"), TEXT("Mutant"), TEXT("Wanderer"),
		TEXT("Devourer"), TEXT("Bloodthirsty"), TEXT("Stitcher"), TEXT("Meateater"),
		TEXT("Ravager"), TEXT("Deathclaw"), TEXT("The Undead"), TEXT("Brainless"),
		TEXT("The Lifeless"), TEXT("Rotten"), TEXT("Filthspawn"), TEXT("Mindless"),
		TEXT("Remnant"), TEXT("Corpsewalker"), TEXT("Cold-Blooded"), TEXT("Scareborn")
	};

	int32 RandomIndex = FMath::RandRange(0, PossibleNames.Num() - 1);
	MobName = PossibleNames[RandomIndex];

	AMyGameStateBase* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameStateBase>() : nullptr;
	if (MyGameState)
	{
		AAIMobController* MobController = Cast<AAIMobController>(GetController());
		if (MobController)
		{
			MyGameState->UpdateLeaderboard(MobName, 0);
		}
	}

	OnRep_MobName(); // Обновляем текст сразу на сервере
}

void AAIMob::MoveToToy(FVector ToyLocation)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->MoveToLocation(ToyLocation);
	}
}

void AAIMob::OnToyPickedUp()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
	}
}

void AAIMob::OnRep_LeaderStatus()
{
	if (!NameTag)
	{
		return;
	}

	if (bIsLeader)
	{
		NameTag->SetTextRenderColor(FColor::Green);
	}
	else
	{
		NameTag->SetTextRenderColor(FColor::Red);
	}
}

void AAIMob::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAIMob, MobName);
	DOREPLIFETIME(AAIMob, bIsLeader);
}