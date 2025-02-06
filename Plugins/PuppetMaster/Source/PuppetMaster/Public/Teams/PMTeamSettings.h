// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Engine/DeveloperSettings.h"
#include "PMTeamSettings.generated.h"

USTRUCT(BlueprintType)
struct FPMTeamAttitudeContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attitude")
	TMap<FGameplayTag, TEnumAsByte<ETeamAttitude::Type>> Attitudes;
};

/*
 * Settings to define teams and attitudes via GameplayTags, for use with the IGenericTeamAgentInterface. Allows for more
 * flexibility than simply defining an enum with generic teams (i.e. Team1, Team2, etc.), but requires more overhead
 * from maintaining maps.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Team Settings"))
class PUPPETMASTER_API UPMTeamSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UPMTeamSettings* Get();

	UFUNCTION(BlueprintPure, Category = "Attitude")
	static ETeamAttitude::Type GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB);

	static FGameplayTag GetTeamTag(const FGenericTeamId& TeamId);
	static FGenericTeamId GetTeamId(const FGameplayTag& TeamTag);

	//~ Begin UDeveloperSettings interface
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UDeveloperSettings interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teams", Config)
	TArray<FGameplayTag> Teams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attitude", Config)
	TMap<FGameplayTag, FPMTeamAttitudeContainer> DefaultTeamAttitudes;

private:
	TMap<FGameplayTag, FGenericTeamId> TeamIds;
	void InitializeTeamIds();
};
