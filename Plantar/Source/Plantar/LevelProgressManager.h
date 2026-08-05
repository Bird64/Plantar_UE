#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelProgressManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnPurificationProgressChanged,
    float,
    NewProgress
);

UCLASS()
class PLANTAR_API ALevelProgressManager : public AActor
{
    GENERATED_BODY()

public:
    ALevelProgressManager();

    /** Blueprint class that all tracked pollution enemies inherit from. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Progress|Enemies")
    TArray<TSubclassOf<AActor>> EnemyClasses;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Progress|Enemies")
    int32 TotalEnemies = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Progress|Enemies")
    int32 EnemiesRemaining = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Progress|Enemies")
    int32 EnemiesDefeated = 0;

    /** Normalized value: 0.0 means nothing purified; 1.0 means fully purified. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Progress")
    float PurificationProgress = 0.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Level Progress|Requirements",
        meta = (ClampMin = "0.0", ClampMax = "1.0")
    )
    float RequiredPurificationPercent = 0.8f;

    UPROPERTY(BlueprintAssignable, Category = "Level Progress")
    FOnPurificationProgressChanged OnPurificationProgressChanged;

    UFUNCTION(BlueprintPure, Category = "Level Progress")
    float GetPurificationProgress() const;

    UFUNCTION(BlueprintPure, Category = "Level Progress")
    bool CanCompleteLevel() const;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle EnemyUpdateTimerHandle;

    void InitializeEnemyTracking();
    void UpdateEnemyProgress();
    int32 CountTrackedEnemies() const;
};