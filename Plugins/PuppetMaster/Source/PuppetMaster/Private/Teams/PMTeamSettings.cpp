// Copyright Alex Jobe


#include "Teams/PMTeamSettings.h"

#include "Logging/PuppetMasterLog.h"

const UPMTeamSettings* UPMTeamSettings::Get()
{
	return GetDefault<UPMTeamSettings>();
}

ETeamAttitude::Type UPMTeamSettings::GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB)
{
	if (TeamA == TeamB) return ETeamAttitude::Friendly;

	const UPMTeamSettings* TeamSettings = Get();
	if (!ensure(TeamSettings)) return ETeamAttitude::Neutral;

	const FGameplayTag TeamTagA = TeamSettings->GetTeamTag(TeamA);
	const FGameplayTag TeamTagB = TeamSettings->GetTeamTag(TeamB);
	if (!TeamTagA.IsValid() || !TeamTagB.IsValid()) return ETeamAttitude::Neutral;

	const FPMTeamAttitudeContainer* TeamAttitudes = TeamSettings->DefaultTeamAttitudes.Find(TeamTagA);
	if (!TeamAttitudes)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPMTeamSettings -- TeamAttitudes not found for TeamTag: %s"), *TeamTagA.ToString());
		return ETeamAttitude::Neutral;
	}

	const TEnumAsByte<ETeamAttitude::Type>* Attitude = TeamAttitudes->Attitudes.Find(TeamTagB);
	if (!Attitude)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPMTeamSettings -- Team %s has no attitude defined towards team %s"), *TeamTagA.ToString(), *TeamTagB.ToString());
		return ETeamAttitude::Neutral;
	}

	return *Attitude;
}

FGameplayTag UPMTeamSettings::GetTeamTag(const FGenericTeamId& TeamId)
{
	const UPMTeamSettings* TeamSettings = Get();
	if (!ensure(TeamSettings)) return FGameplayTag();
	
	FGameplayTag TeamTag = FGameplayTag();
	if (TeamId.GetId() < TeamSettings->Teams.Num())
	{
		TeamTag = TeamSettings->Teams[TeamId.GetId()];
	}

	if (!TeamTag.IsValid())
	{
		UE_LOG(LogPuppetMaster, Warning, TEXT("UPMTeamSettings -- Invalid TeamTag for TeamId: %i"), TeamId.GetId());
	}
	
	return TeamTag;
}

FGenericTeamId UPMTeamSettings::GetTeamId(const FGameplayTag& TeamTag)
{
	const UPMTeamSettings* TeamSettings = Get();
	if (!ensure(TeamSettings)) return FGenericTeamId();
	
	if (const FGenericTeamId* TeamId = TeamSettings->TeamIds.Find(TeamTag))
	{
		return *TeamId;
	}

	UE_LOG(LogPuppetMaster, Warning, TEXT("UPMTeamSettings -- Invalid TeamId for TeamTag: %s"), *TeamTag.ToString());
	return FGenericTeamId();
}

void UPMTeamSettings::PostInitProperties()
{
	Super::PostInitProperties();
	InitializeTeamIds();
}

void UPMTeamSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr) return;

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPMTeamSettings, Teams))
	{
		InitializeTeamIds();
	}
}

void UPMTeamSettings::InitializeTeamIds()
{
	TeamIds.Empty();

	// TeamId 255 reserved for NoTeam
	constexpr int MAX_TEAMS = 255;

	const int NumTeams = FMath::Min(Teams.Num(), MAX_TEAMS);
	if (Teams.Num() > MAX_TEAMS)
	{
		UE_LOG(LogPuppetMaster, Warning, TEXT("UPMTeamSettings -- Too many teams defined"));
	}
	
	for (int Index = 0; Index < NumTeams; ++Index)
	{
		TeamIds.Add(Teams[Index], FGenericTeamId(Index));
	}
}
