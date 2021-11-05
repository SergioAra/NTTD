
#include "Item.h"
#include "NTTDCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
//#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	bIsPlayerOverlapping(false),
	ItemState(EItemState::EIS_Pickup),
	//Item interp variables
	IntemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ZCurveTime(0.7f),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f)

{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	//set the itemMesh as the root component
	SetRootComponent(ItemMesh);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	//block collision to all channels
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//set collision to block visibility channel
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	//PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	//PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//hide pickup widget on start
	/*
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
*/
	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//sets active stars array based on item rarity
	SetActiveStars();

	//Set item properties based on ItemState
	SetItemProperties(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		ANTTDCharacter* NTTDCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(NTTDCharacter)
		{
			NTTDCharacter->IncrementOverlappedItemCount(1);
			bIsPlayerOverlapping = true;
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		ANTTDCharacter* NTTDCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(NTTDCharacter)
		{
			NTTDCharacter->IncrementOverlappedItemCount(-1);
			bIsPlayerOverlapping = false;
		}
	}
}

void AItem::SetActiveStars()
{
	
	for(int32 i = 0; i<=5; i++)
	{
		ActiveStars.Add(false);
	}

	switch(ItemRarity)
	{
		case EItemRarity::EIR_Mythic:
			ActiveStars[5] = true;
		case EItemRarity::EIR_Legendary:
			ActiveStars[4] = true;
		case EItemRarity::EIR_Epic:
			ActiveStars[3] = true;
		case EItemRarity::EIR_Rare:
			ActiveStars[2] = true;
		case EItemRarity::EIR_Uncommon:
			ActiveStars[1] = true;
		case EItemRarity::EIR_Common:
			ActiveStars[0] = true;
		default:

			break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch(State)
	{
		case EItemState::EIS_Pickup:
			//Set ItemMesh properties
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Set AreaSphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//Set CollisionBox properties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		case EItemState::EIS_EquipInterping:
		case EItemState::EIS_Equipped:
			//PickupWidget->SetVisibility(false);
			
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_Falling:
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetEnableGravity(true);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			ItemMesh->SetVisibility(true);

			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;

		case EItemState::EIS_PickedUp:
			
			break;
		default:

			break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if(Character)
	{
		Character->GetPickupItem(this);
	}
}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}






