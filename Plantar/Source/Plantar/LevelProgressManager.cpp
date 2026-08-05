#include "LevelProgressManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ALevelProgressManager::ALevelProgressManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALevelProgressManager::BeginPlay()
{
    Super::BeginPlay();

    InitializeEnemyTracking();

    // Quick, safe integration with existing Blueprint enemies.
    // We can replace this with explicit death events later.
    GetWorldTimerManager().SetTimer(
        EnemyUpdateTimerHandle,
        this,
        &ALevelProgressManager::UpdateEnemyProgress,
        0.25f,
        true
    );
}

void ALevelProgressManager::InitializeEnemyTracking()
{
    if (EnemyClasses.IsEmpty())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("LevelProgressManager: No EnemyClasses are assigned.")
        );

        return;
    }

    TotalEnemies = CountTrackedEnemies();
    EnemiesRemaining = TotalEnemies;
    EnemiesDefeated = 0;
    PurificationProgress = TotalEnemies > 0 ? 0.0f : 1.0f;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("LevelProgressManager found %d enemies across %d configured classes."),
        TotalEnemies,
        EnemyClasses.Num()
    );

    OnPurificationProgressChanged.Broadcast(PurificationProgress);
}

void ALevelProgressManager::UpdateEnemyProgress()
{
    if (EnemyClasses.IsEmpty())
    {
        return;
    }

    const float PreviousProgress = PurificationProgress;

    EnemiesRemaining = CountTrackedEnemies();

    EnemiesDefeated = FMath::Max(
        0,
        TotalEnemies - EnemiesRemaining
    );

    PurificationProgress =
        TotalEnemies > 0
            ? static_cast<float>(EnemiesDefeated) /
                static_cast<float>(TotalEnemies)
            : 1.0f;

    PurificationProgress = FMath::Clamp(
        PurificationProgress,
        0.0f,
        1.0f
    );

    if (!FMath::IsNearlyEqual(
        PreviousProgress,
        PurificationProgress
    ))
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("Purification: %d/%d enemies defeated, %.0f%%"),
            EnemiesDefeated,
            TotalEnemies,
            PurificationProgress * 100.0f
        );

        OnPurificationProgressChanged.Broadcast(
            PurificationProgress
        );
    }

    if (GEngine)
    {
        const FString ProgressMessage = FString::Printf(
            TEXT("Area Purified: %d/%d enemies (%d%%)"),
            EnemiesDefeated,
            TotalEnemies,
            FMath::RoundToInt(PurificationProgress * 100.0f)
        );

        GEngine->AddOnScreenDebugMessage(
            1001,
            0.35f,
            FColor::Green,
            ProgressMessage
        );
    }
}

int32 ALevelProgressManager::CountTrackedEnemies() const
{
    if (!GetWorld())
    {
        return 0;
    }

    int32 EnemyCount = 0;

    for (const TSubclassOf<AActor>& TrackedEnemyClass : EnemyClasses)
    {
        if (!TrackedEnemyClass)
        {
            continue;
        }

        TArray<AActor*> FoundEnemies;

        UGameplayStatics::GetAllActorsOfClass(
            GetWorld(),
            TrackedEnemyClass,
            FoundEnemies
        );

        EnemyCount += FoundEnemies.Num();
    }

    return EnemyCount;
}

float ALevelProgressManager::GetPurificationProgress() const
{
    return PurificationProgress;
}

bool ALevelProgressManager::CanCompleteLevel() const
{
    return PurificationProgress >= RequiredPurificationPercent;
}