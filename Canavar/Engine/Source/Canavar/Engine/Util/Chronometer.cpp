#include "Chronometer.h"

#include <format>

Canavar::Engine::Chronometer::Chronometer(const std::string& Name)
    : mName(Name)
{}

Canavar::Engine::Chronometer::~Chronometer()
{
    std::scoped_lock Lock(sMutex);

    const auto Now = std::chrono::system_clock::now();

    // Update
    auto& Stats = sStats[mName];
    const auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(Now - mStartTime);

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

std::string Canavar::Engine::Chronometer::PrintAll()
{
    std::string Result;

    for (const auto& [Name, Stats] : sStats)
    {
        Result += Print(Name) + "\n";
    }

    return Result;
}

std::mutex Canavar::Engine::Chronometer::sMutex{};

std::map<std::string, Canavar::Engine::Stats> Canavar::Engine::Chronometer::sStats{};