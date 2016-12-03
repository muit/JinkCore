// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Faction.h"
#include "Spell.h"
#include "Entity.generated.h"

typedef class ABasic_Con;
class UItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJustDiedSignature, AController*, InstigatedBy, AEntity*, Killer);

//MovementState to define different types of movements.
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	MS_None	    UMETA(DisplayName = "None"),
	MS_Walk 	UMETA(DisplayName = "Walk"),
	MS_Run  	UMETA(DisplayName = "Run")
};


UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "An entity is a type of Character that includes the basic alive features."))
class JINKCORE_API AEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	/**
	 * PROPERTIES
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	FFaction Faction;


    /**
    * Begin ATTRIBUTES
    */
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Attributes")
    float Live;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Attributes")
    float MaxLive;

    /** Base Damage. */
    UPROPERTY(EditAnywhere, Category = "Entity|Attributes")
    float Damage;

    /** Fire rate in shots/second. */
    UPROPERTY(EditAnywhere, Category = "Entity|Attributes", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    float FireRate;

    /** Adquired items. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attributes", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    TArray<TSubclassOf<UItem>> Items;


    /**
     * Get the actor end damage.
     * @return Modified damage.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Attributes")
    float GetDamage() const;
    /**
     * Get the actor base damage.
     * @return Base damage.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Attributes")
    float GetBaseDamage() const { return Damage; }
    /**
     * Get the actor end fire rate.
     * @return Modified fire rate.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Attributes")
    float GetFireRate() const;
    /**
     * Get the actor base fire rate.
     * @return Base fire rate.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Attributes")
    float GetBaseFireRate() const { return FireRate; }
    /**
     * Add an Item to the entity.
     * @param Class	The class of the item.
     * @return      Id of the added item. -1 if error.
     */
    UFUNCTION(BlueprintCallable, Category = "Entity|Attributes")
    int32 AddItem(TSubclassOf<UItem> Class);
    /**
     * Remove an Item from the entity.
     * @param Class	The class of the removed item.
     */
    UFUNCTION(BlueprintCallable, Category = "Entity|Attributes")
    void RemoveItem(TSubclassOf<UItem> Class);
    /** Remove an Item by Id from the entity.
     * @param Id Id of the item to be removed.
     */
    UFUNCTION(BlueprintCallable, Category = "Entity|Attributes", meta = (DisplayName = "Remove Item"))
    void RemoveItemById(int32 Id);
    /**
    * Remove all Items of a class from the entity.
    * @param Class The class of the removed items.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Attributes")
    void RemoveAllItems(TSubclassOf<UItem> Class);
    /**
    * Remove all items.
    * @param Class	The class of the removed items.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Attributes")
    void ClearItems();

    /* End ATTRIBUTES*/



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Movement")
	EMovementState MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float RunSpeed;


	UCharacterMovementComponent* CharacterMovement;

	//Set Movement to Walk
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Walk();
	//Set Movement to Run
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Run();
	//Update Movement Speed to the movement State
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void UpdateMovementSpeed();
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	void RotateTowards(FRotator Rotation);
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	void RotateTowardsActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	virtual bool IsAlive() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	virtual bool LiveIsUnderPercent(float Percent) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsHostileTo(AEntity* Other);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsHostileToFaction(FFaction Other);

	UFUNCTION(BlueprintCallable, Category = "Entity")
	void Die(AController * InstigatedBy = nullptr, AEntity* Killer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Entity")
	ASpell* CastSpell(TSubclassOf<ASpell> SpellType, AEntity* Target, FVector Location, FRotator Rotation, float _Damage = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "Entity")
	ASpell* CastSpellAtCaster(TSubclassOf<ASpell> SpellType, AEntity* Target = NULL, float _Damage = 0.0f);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void ReceiveDamage(AActor* DamagedActor, float _Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void DoMeleAttack(AEntity* Target);



	/**********
	* DAMAGES *
	**********/

	/** Hurt locally authoritative actors within the radius. Will only hit components that block the Visibility channel.
	* @param Damage - The base damage to apply, i.e. the damage at the origin.
	* @param Origin - Epicenter of the damage area.
	* @param DamageRadius - Radius of the damage area, from Origin
	* @param DamageTypeClass - Class that describes the damage that was done.
	* @param DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded).  This actor will not be damaged and it will not block damage.
	* @param bFullDamage - if true, damage not scaled based on distance from Origin
	* @param DamagePreventionChannel - Damage will not be applied to victim if there is something between the origin and the victim which blocks traces on this channel
	* @return true if damage was applied to at least one actor.
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Entity|Damage", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
	bool ApplyRadialDamage(float _Damage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser = NULL, bool bDoFullDamage = false, ECollisionChannel DamagePreventionChannel = ECC_Visibility);
	
	/** Hurt locally authoritative actors within the radius. Will only hit components that block the Visibility channel.
	* @param Damage - The base damage to apply, i.e. the damage at the origin.
	* @param Origin - Epicenter of the damage area.
	* @param DamageInnerRadius - Radius of the full damage area, from Origin
	* @param DamageOuterRadius - Radius of the minimum damage area, from Origin
	* @param DamageFalloff - Falloff exponent of damage from DamageInnerRadius to DamageOuterRadius
	* @param DamageTypeClass - Class that describes the damage that was done.
	* @param IgnoreActors - Actors ignored.
	* @param DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded)
	* @param DamagePreventionChannel - Damage will not be applied to victim if there is something between the origin and the victim which blocks traces on this channel
	* @return true if damage was applied to at least one actor.
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Entity|Damage", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
	bool ApplyRadialDamageWithFalloff(float _Damage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser = NULL, ECollisionChannel DamagePreventionChannel = ECC_Visibility);
	

	/** Hurts the specified actor with the specified impact.
	* @param DamagedActor - Actor that will be damaged.
	* @param Damage - The base damage to apply.
	* @param HitFromDirection - Direction the hit came FROM
	* @param HitInfo - Collision or trace result that describes the hit
	* @param EventInstigator - Controller that was responsible for causing this damage (e.g. player who shot the weapon)
	* @param DamageTypeClass - Class that describes the damage that was done.
	* @param DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded)
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Entity|Damage")
	void ApplyPointDamage(AActor* DamagedActor, float _Damage, const FVector& HitFromDirection, const FHitResult& HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser = NULL);
	
	/** Hurts the specified actor with generic damage.
	* @param DamagedActor - Actor that will be damaged.
	* @param Damage - The base damage to apply.
	* @param DamageTypeClass - Class that describes the damage that was done.
	* @param DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded)
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Entity|Damage")
	void ApplyDamage(AActor* DamagedActor, float _Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser = NULL);
	
	// APPLY CHECKS
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckApplyRadialDamage(float _Damage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel);
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckApplyRadialDamageWithFalloff(float _Damage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, ECollisionChannel DamagePreventionChannel);
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckApplyPointDamage(AActor* DamagedActor, float _Damage, const FVector& HitFromDirection, const FHitResult& HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser);
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckApplyDamage(AActor* DamagedActor, float _Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser);
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckApplyAnyDamage(AActor* DamagedActor, float _Damage, const class UDamageType * DamageType, AActor* DamageCauser);

	// RECEIVE CHECKS
	UFUNCTION(BlueprintNativeEvent, Category = "Entity|Damage")
	bool CheckReceiveDamage(AActor* DamagedActor, float _Damage, const class UDamageType * DamageType, AActor* DamageCauser);


	//EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void JustDied(AController * InstigatedBy, AEntity* Killer);
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FJustDiedSignature JustDiedDelegate;
	
	/**
	 * HANDLERS
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	ABasic_Con* GetAI() const { return Cast<ABasic_Con>(GetController()); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsPlayer() const { return IsPlayerControlled(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsAI() const { return GetAI() != NULL; }
};
