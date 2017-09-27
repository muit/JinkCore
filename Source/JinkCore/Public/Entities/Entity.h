// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Spell.h"
#include "Buff.h"

#include "Faction.h"
#include "FactionAgentInterface.h"

#include "Entity.generated.h"

struct FActorSpawnParameters;
class UItem;
class USummonList;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJustDiedSignature, AController*, InstigatedBy, AEntity*, Killer);

//MovementState to define different types of movements.
UENUM(BlueprintType)
enum class EMovementState : uint8
{
    MS_None     UMETA(DisplayName = "None"),
    MS_Walk     UMETA(DisplayName = "Walk"),
    MS_Run      UMETA(DisplayName = "Run")
};


UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "An entity is a type of Character that includes the basic alive features."))
class JINKCORE_API AEntity : public ACharacter, public IFactionAgentInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEntity();

    virtual void OnConstruction(const FTransform & Transform) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attributes")
    FEntityAttribute MaxLive;
    /** Base Damage. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attributes")
    FEntityAttribute Damage;
    /** Fire rate in shots/second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attributes")
    FEntityAttribute FireRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attributes", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    FEntityAttribute BulletSpeed;

    /** Adquired items at start */
    UPROPERTY(EditAnywhere, Category = "Entity|Attributes", meta = (DisplayName = "Items"))
    TArray<TSubclassOf<UItem>> ItemsAtStart;
    /** Adquired item objects */
    UPROPERTY(BlueprintReadOnly, Category = "Entity|Items")
    TArray<UItem*> Items;
    /* End ATTRIBUTES*/


    /**
    * Begin ITEMS
    */

    /**
    * Add an Item to the entity.
    * @param Class The class of the item.
    * @return      Id of the added item. -1 if error.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Items")
    UItem* PickUpItem(TSubclassOf<UItem> Type);

    UFUNCTION(BlueprintImplementableEvent, Category = "Entity|Items")
    void OnItemPickUp(UItem* Item);

    /**
    * Remove an Item from the entity.
    * @param Class    The class of the removed item.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Items")
    void DropItem(UItem* Type);

    /**
    * Remove all Items of a class from the entity.
    * @param Type The class of the removed items.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Items")
    void DropAllItems(TSubclassOf<UItem> Type);

    /**
    * Remove all items.
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Items")
    void ClearItems();

    /**
     * Checks if entity has an Item of certain type
     * @param Type Type of the entity.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Items")
    bool HasItem(TSubclassOf<UItem> Type);

    UFUNCTION(BlueprintImplementableEvent, Category = "Entity|Items")
    void OnItemDrop(UItem* Item);
    /* End ITEMS*/


    UCharacterMovementComponent* CharacterMovement;

private:

    UPROPERTY(EditAnywhere, Category = "Entity|Movement")
    EMovementState MovementState;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
    FEntityAttribute WalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
    FEntityAttribute RunSpeed;


    //Set Movement to Walk
    UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
    void Walk() { SetMovementState(EMovementState::MS_Walk); }

    //Set Movement to Run
    UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
    void Run() { SetMovementState(EMovementState::MS_Run); }

    //Set Movement to any state
    UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
    void SetMovementState(const EMovementState State);

    //Get Current movement state
    UFUNCTION(BlueprintPure, Category = "Entity|Movement")
    FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

    UFUNCTION()
    void OnMovementAttributeModified(const EAttributeOpType Operation, const FAttributeModification& Modification) {
        //Update movement Speed
        SetMovementState(MovementState);
    }


    UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
    void RotateTowards(FRotator Rotation);
    UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
    void RotateTowardsActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    virtual bool IsAlive() const;
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    virtual bool LiveIsUnderPercent(float Percent) const;



    /***************************************
    * Factions                             *
    ***************************************/

    /** Retrieve faction identifier in form of Faction */
    UFUNCTION(BlueprintPure, Category = Faction)
    virtual FFaction GetFaction() const override;

    /** Assigns faction */
    UFUNCTION(BlueprintCallable, Category = Faction)
    virtual void SetFaction(const FFaction& InFaction) override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    bool IsHostileTo(const AActor* Other);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    bool IsHostileToFaction(const FFaction Other);



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
    void JustDied_Internal(AController * InstigatedBy, AEntity* Killer);

    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void JustDied(AController * InstigatedBy, AEntity* Killer);
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FJustDiedSignature JustDiedDelegate;
    
    /**
     * HANDLERS
     */
    /*UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    ABasic_Con* GetAI() const { return Cast<ABasic_Con>(GetController()); }*/

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
    bool IsPlayer() const { return IsPlayerControlled(); }

    /*UFUNCTION(BlueprintPure, Category = "Entity")
    bool IsAI() const { return GetAI() != NULL; }*/

    /**
     * DROPS
     */
    //Drop SelectionQuery

    //void DropItem(TAssetPtr<USelectionQuery> SelectionQuery);

    /**
    * BUFFS / DEBUFFS
    */
    UPROPERTY(EditAnywhere, Category = "Entity|Attributes", meta = (DisplayName = "Buffs"))
    TArray<TSubclassOf<UBuff>> BuffsAtStart;
    UPROPERTY(BlueprintReadOnly, Category = "Entity|Buffs")
    TArray<UBuff*> Buffs;

    UFUNCTION(BlueprintCallable, Category = "Entity|Buffs")
    UBuff* ApplyBuff(TSubclassOf<UBuff> Class);

    UFUNCTION(BlueprintCallable, Category = "Entity|Buffs")
    void RemoveBuff(UBuff* Buff);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Buffs")
    bool HasBuff(UBuff* Buff);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Buffs")
    bool HasBuffOfClass(TSubclassOf<UBuff> Class);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Buffs")
    const TArray<UBuff*>& GetBuffs();

    /**
    * SUMMONING
    */
protected:
    UPROPERTY(VisibleAnywhere, Category = "Entity")
    bool bIsSummoned;
    UPROPERTY(BlueprintReadOnly, Category = "Entity")
    AEntity* Summoner;

public:
    /**
    * Summon Entities with given absolute transform (override root component transform) and SpawnParameters
    *
    * @param	Class					Class to summon, child of AEntity
    * @param	Transform		        World Transform to spawn on
    * @param	SpawnParameters			Spawn Parameters
    *
    * @return	Summoned Entity
    */
    UFUNCTION(BlueprintCallable, Category = "Entity|Summons")
    AEntity* Summon(UClass* Class, FTransform Transform);

    /** Templated version of Summon that allows you to specify a class type via the template type */
    template<class T>
    T Summon(FTransform Transform);
    

    void SetupSummon(AEntity* InSummoner);

    UFUNCTION(BlueprintCallable, Category = "Entity|Summons")
    USummonList* CreateSummonList();


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Summons")
    bool IsSummoned() const { return bIsSummoned && Summoner; };
    /**
    * Returns the summoner of the entity if it was summoned.
    */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Summons")
    AEntity* GetSummoner() { return bIsSummoned? Summoner : nullptr; };

    UFUNCTION(BlueprintImplementableEvent, Category = "Entity|Summons")
    void JustSummoned(AEntity* _Summoner);
};
