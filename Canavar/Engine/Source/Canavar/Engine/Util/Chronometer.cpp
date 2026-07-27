#include "Chronometer.h"

#include <algorithm>
#include <format>
#include <vector>

Canavar::Engine::Chronometer::Chronometer(const std::string& Name)
    : mName(Name)
{}

Canavar::Engine::Chronometer::~Chronometer()
{
    const auto Now = std::chrono::system_clock::now();
    const auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(Now - mStartTime);

    auto& Stats = sStats[mName];

    Stats.LastCallTime = Duration;
    Stats.TotalCallTime += Duration;
    Stats.NumberOfCalls += 1;

    if (Stats.LongestCallTime < Duration)
    {
        Stats.LongestCallTime = Duration;
    }
}

std::string Canavar::Engine::Chronometer::Print(const std::string& Name)
{
    const auto& Stats = sStats[Name];

    return std::format("{:<45}: {:<5.3} ms,      {:<5.3} ms,      {:<5.3} sec", //
                       Name,
                       Stats.LastCallTime.count() / 1'000.0f,
                       Stats.LongestCallTime.count() / 1'000.0f,
                       Stats.TotalCallTime.count() / 1'000'000.0f);
}

std::string Canavar::Engine::Chronometer::PrintAll(SortBy Sort)
{
    std::string Result;

    switch (Sort)
    {
    case SortBy::Name:
    default:
        for (const auto& [Name, Stats] : sStats)
        {
            Result += Print(Name) + "\n";
        }
        break;
    case SortBy::TotalCallTime:
    {
        std::vector<std::pair<std::string, Stats>> SortedStats(sStats.begin(), sStats.end());
        std::sort(SortedStats.begin(), SortedStats.end(), [](const auto& A, const auto& B) { return A.second.TotalCallTime > B.second.TotalCallTime; });
        for (const auto& [Name, Stats] : SortedStats)
        {
            Result += Print(Name) + "\n";
        }

        break;
    }
    }

    return Result;
}

std::vector<Canavar::Engine::Chronometer::Entry> Canavar::Engine::Chronometer::GetAllStats(SortBy Sort)
{
    std::vector<Entry> Entries;
    Entries.reserve(sStats.size());

    for (const auto& [Name, Stats] : sStats)
    {
        Entries.push_back({ Name, Stats });
    }

    if (Sort == SortBy::TotalCallTime)
    {
        std::sort(Entries.begin(), Entries.end(), [](const Entry& A, const Entry& B) {
            return A.Stats.TotalCallTime > B.Stats.TotalCallTime;
        });
    }

    return Entries;
}

std::map<std::string, Canavar::Engine::Chronometer::Stats> Canavar::Engine::Chronometer::sStats{};